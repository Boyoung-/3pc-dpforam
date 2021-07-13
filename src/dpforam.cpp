#include "dpforam.h"

#include <assert.h>
#include <omp.h>

#include <iostream>

#include "insert_label.h"
#include "ssot.h"
#include "util.h"

fss1bit dpforam::fss;

void dpforam::init() {
    init_ctr();
    set_zero(rom[0]);
    set_zero(rom[1]);
    set_zero(wom);
    if (!isFirst) {
        pos_map->init();
    }
}

void dpforam::init_ctr() {
    stash_ctr = 1;
}

void dpforam::set_zero(uchar **mem) {
    if (mem == NULL) {
        return;
    }
#pragma omp parallel for
    for (unsigned long i = 0; i < N; i++) {
        memset(mem[i], 0, DBytes);
    }
}

void dpforam::init_mem(uchar **&mem) {
    mem = new uchar *[N];
    for (unsigned long i = 0; i < N; i++) {
        mem[i] = new uchar[DBytes];
    }
}

void dpforam::delete_mem(uchar **mem) {
    for (unsigned long i = 0; i < N; i++) {
        delete[] mem[i];
    }
    delete[] mem;
}

void dpforam::block_pir(const unsigned long addr_23[2],
                        const uchar *const *const mem_23[2], unsigned long size, uchar *block_23[2],
                        uchar *fss_out[2]) {
    uchar *keys[2];
    uint keyBytes = fss.gen(addr_23[0] ^ addr_23[1], logN, keys);
    cons[0]->write(keys[0], keyBytes);
    cons[1]->write(keys[1], keyBytes);
    cons[0]->read(keys[1], keyBytes);
    cons[1]->read(keys[0], keyBytes);

    uint quo = DBytes / 16;
    uint rem = DBytes % 16;
    memset(block_23[0], 0, DBytes);

    if (omp_get_max_threads() == 1) {
        for (uint i = 0; i < 2; i++) {
            fss.eval_all_with_perm(keys[i], logN, addr_23[i], fss_out[i]);
            for (unsigned long j = 0; j < size; j++) {
                //				if (fss_out[i][j])
                {
                    //					set_xor_128(mem_23[i][j], quo, rem, block_23[0]);
                    select_xor_128(mem_23[i][j], fss_out[i][j], quo, rem, block_23[0]);
                }
            }
        }
    } else {
#pragma omp parallel
        {
#pragma omp for
            for (uint i = 0; i < 2; i++) {
                fss.eval_all_with_perm(keys[i], logN, addr_23[i], fss_out[i]);
            }

            uchar tmp[DBytes];
            memset(tmp, 0, DBytes * sizeof(uchar));
#pragma omp for collapse(2)
            for (uint i = 0; i < 2; i++) {
                for (unsigned long j = 0; j < size; j++) {
                    //					if (fss_out[i][j])
                    {
                        //						set_xor_128(mem_23[i][j], quo, rem, tmp);
                        select_xor_128(mem_23[i][j], fss_out[i][j], quo, rem, tmp);
                    }
                }
            }
#pragma omp critical
            {
                set_xor_128(tmp, quo, rem, block_23[0]);
            }
        }
    }

    cons[0]->write(block_23[0], DBytes);
    cons[1]->read(block_23[1], DBytes);

    delete[] keys[0];
    delete[] keys[1];
}

void dpforam::rec_pir(const uint idx_23[2], const uchar *const block_23[2],
                      uchar *rec_23[2]) {
    uchar *keys[2];
    uint keyBytes = fss.gen(idx_23[0] ^ idx_23[1], tau, keys);
    cons[0]->write(keys[0], keyBytes);
    cons[1]->write(keys[1], keyBytes);
    cons[0]->read(keys[1], keyBytes);
    cons[1]->read(keys[0], keyBytes);

    memset(rec_23[0], 0, nextLogNBytes);
    for (uint i = 0; i < 2; i++) {
        uchar fss_out[ttp];
        fss.eval_all(keys[i], tau, fss_out);
        for (uint j = 0; j < ttp; j++) {
            if (fss_out[j ^ idx_23[i]]) {
                cal_xor(rec_23[0], block_23[i] + j * nextLogNBytes,
                        nextLogNBytes, rec_23[0]);
            }
        }
    }

    cons[0]->write(rec_23[0], nextLogNBytes);
    cons[1]->read(rec_23[1], nextLogNBytes);

    delete[] keys[0];
    delete[] keys[1];
}

void dpforam::gen_delta_array(const uint idx_23[2], uint numChunk,
                              uint chunkBytes, const uchar *const delta_23[2],
                              uchar *delta_array_23[2]) {
    uint arrayBytes = numChunk * chunkBytes;
    insert_label il(party, cons, rnd, prgs);
    if (strcmp(party, "eddie") == 0) {
        uchar L1[chunkBytes];
        cal_xor(delta_23[0], delta_23[1], chunkBytes, L1);
        il.runE(idx_23[0] ^ idx_23[1], L1, numChunk, chunkBytes);
        prgs[0].GenerateBlock(delta_array_23[0], arrayBytes);
        prgs[1].GenerateBlock(delta_array_23[1], arrayBytes);
    } else if (strcmp(party, "debbie") == 0) {
        il.runD(idx_23[0], delta_23[0], numChunk, chunkBytes,
                delta_array_23[0]);
        prgs[1].GenerateBlock(delta_array_23[1], arrayBytes);
        cal_xor(delta_array_23[0], delta_array_23[1], arrayBytes,
                delta_array_23[0]);
        cons[0]->write(delta_array_23[0], arrayBytes);
        uchar tmp[arrayBytes];
        cons[0]->read(tmp, arrayBytes);
        cal_xor(delta_array_23[0], tmp, arrayBytes, delta_array_23[0]);
    } else if (strcmp(party, "charlie") == 0) {
        il.runC(numChunk, chunkBytes, delta_array_23[1]);
        prgs[0].GenerateBlock(delta_array_23[0], arrayBytes);
        cal_xor(delta_array_23[0], delta_array_23[1], arrayBytes,
                delta_array_23[1]);
        cons[1]->write(delta_array_23[1], arrayBytes);
        uchar tmp[arrayBytes];
        cons[1]->read(tmp, arrayBytes);
        cal_xor(delta_array_23[1], tmp, arrayBytes, delta_array_23[1]);
    } else {
    }
}

void dpforam::obliv_select(const uchar *const rom_block_23[2],
                           const uchar *const stash_block_23[2], const uchar indicator_23[2],
                           uchar *block_23[2]) {
    ssot ot(party, cons, rnd, prgs);
    if (strcmp(party, "eddie") == 0) {
        uint b1 = (indicator_23[0] ^ indicator_23[1]) & 1;
        uchar *v01[2] = {new uchar[DBytes], new uchar[DBytes]};
        cal_xor(rom_block_23[0], rom_block_23[1], DBytes, v01[0]);
        cal_xor(stash_block_23[0], stash_block_23[1], DBytes, v01[1]);
        ot.runE(b1, v01, DBytes, block_23[1]);
        prgs[0].GenerateBlock(block_23[0], DBytes);
        cal_xor(block_23[0], block_23[1], DBytes, block_23[1]);
        cons[1]->write(block_23[1], DBytes);
        uchar tmp[DBytes];
        cons[1]->read(tmp, DBytes);
        cal_xor(block_23[1], tmp, DBytes, block_23[1]);
        delete[] v01[0];
        delete[] v01[1];
    } else if (strcmp(party, "debbie") == 0) {
        ot.runD(DBytes);
        prgs[0].GenerateBlock(block_23[0], DBytes);
        prgs[1].GenerateBlock(block_23[1], DBytes);
    } else if (strcmp(party, "charlie") == 0) {
        int b0 = indicator_23[1] & 1;
        const uchar *u01[2] = {rom_block_23[1], stash_block_23[1]};
        ot.runC(b0, u01, DBytes, block_23[0]);
        prgs[1].GenerateBlock(block_23[1], DBytes);
        cal_xor(block_23[0], block_23[1], DBytes, block_23[0]);
        cons[0]->write(block_23[0], DBytes);
        uchar tmp[DBytes];
        cons[0]->read(tmp, DBytes);
        cal_xor(block_23[0], tmp, DBytes, block_23[0]);
    } else {
    }
}

void dpforam::update_wom(const uchar *const delta_block_23[2],
                         const uchar *const fss_out[2]) {
    uint quo = DBytes / 16;
    uint rem = DBytes % 16;
#pragma omp parallel for
    for (unsigned long j = 0; j < N; j++) {
        for (uint i = 0; i < 2; i++) {
            //			if (fss_out[i][j])
            {
                //set_xor_128(delta_block_23[i], quo, rem, wom[j]);
                select_xor_128(delta_block_23[i], fss_out[i][j], quo, rem, wom[j]);
            }
        }
    }
}

void dpforam::append_stash(const uchar *const block_23[2],
                           const uchar *const delta_block_23[2]) {
    for (uint i = 0; i < 2; i++) {
        cal_xor(block_23[i], delta_block_23[i], DBytes, stash[i][stash_ctr]);
    }
    stash_ctr++;
    if (stash_ctr == N) {
        init_ctr();
        wom_to_rom();
        pos_map->init();
    }
}

// TODO: buffered read/write
void dpforam::wom_to_rom() {
    if (isFirst) {
        return;
    }
    for (unsigned long i = 0; i < N; i++) {
        memcpy(rom[0][i], wom[i], DBytes);
    }
    for (unsigned long i = 0; i < N; i++) {
        cons[0]->write(wom[i], DBytes);
        //		cons[0]->fwrite(wom[i], DBytes);
    }
    //	cons[0]->flush();
    for (unsigned long i = 0; i < N; i++) {
        cons[1]->read(rom[1][i], DBytes);
        //		cons[1]->fread(rom[1][i], DBytes);
    }
}

dpforam::dpforam(const char *party, connection *cons[2],
                 CryptoPP::AutoSeededRandomPool *rnd,
                 CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption *prgs, uint tau,
                 uint logN, uint DBytes, bool isLast) : protocol(party, cons, rnd, prgs) {
    this->isLast = isLast;
    this->tau = isLast ? std::max(5 - (int)log2(DBytes), 0) : tau;
    this->logN = (logN <= this->tau || !isLast) ? logN : (logN - this->tau);
    ttp = 1 << this->tau;
    logNBytes = (this->logN + 7) / 8 + 1;
    nextLogN = isLast ? 0 : logN + tau;
    nextLogNBytes = isLast ? DBytes : (nextLogN + 7) / 8 + 1;
    this->DBytes = nextLogNBytes * ttp;
    N = 1ul << this->logN;
    isFirst = this->logN < 2 * tau;

    init_mem(rom[0]);
    init_mem(rom[1]);
    if (isFirst) {
        wom = NULL;
        pos_map = NULL;
    } else {
        init_mem(wom);
        init_mem(stash[0]);
        init_mem(stash[1]);
        pos_map = new dpforam(party, cons, rnd, prgs, tau, this->logN - tau, 0,
                              false);
    }
    init_ctr();

    if (isLast) {
        init();
    }
}

dpforam::~dpforam() {
    if (!isFirst) {
        delete pos_map;
        delete_mem(stash[0]);
        delete_mem(stash[1]);
        delete_mem(wom);
    }
    delete_mem(rom[0]);
    delete_mem(rom[1]);
}

void dpforam::access(const unsigned long addr_23[2], const uchar *const new_rec_23[2],
                     bool isRead, uchar *rec_23[2]) {
    uint mask = ttp - 1;
    unsigned long addrPre_23[2];
    uint addrSuf_23[2];
    for (uint i = 0; i < 2; i++) {
        addrPre_23[i] = addr_23[i] >> tau;
        addrSuf_23[i] = (uint)addr_23[i] & mask;
    }

    if (isFirst) {
        uchar *block_23[2];
        uchar *fss_out[2];
        uchar *delta_rec_23[2];
        uchar *delta_block_23[2];
        uchar *delta_rom_23[2];
        for (uint i = 0; i < 2; i++) {
            block_23[i] = new uchar[DBytes];
            fss_out[i] = new uchar[N];
            delta_rec_23[i] = new uchar[nextLogNBytes];
            delta_block_23[i] = new uchar[DBytes];
            delta_rom_23[i] = new uchar[N * DBytes];
        }
        block_pir(addrPre_23, rom, N, block_23, fss_out);
        rec_pir(addrSuf_23, block_23, rec_23);

        for (uint i = 0; i < 2; i++) {
            if (isRead) {
                memset(delta_rec_23[i], 0, nextLogNBytes);
            } else {
                cal_xor(rec_23[i], new_rec_23[i], nextLogNBytes,
                        delta_rec_23[i]);
            }
        }
        gen_delta_array(addrSuf_23, ttp, nextLogNBytes, delta_rec_23,
                        delta_block_23);

        uint int_addrPre_23[2] = {(uint)addrPre_23[0], (uint)addrPre_23[1]};
        gen_delta_array(int_addrPre_23, (uint)N, DBytes, delta_block_23,
                        delta_rom_23);

        for (uint i = 0; i < 2; i++) {
            for (unsigned long j = 0; j < N; j++) {
                cal_xor(rom[i][j], delta_rom_23[i] + j * DBytes, DBytes,
                        rom[i][j]);
            }
        }

        for (uint i = 0; i < 2; i++) {
            delete[] block_23[i];
            delete[] fss_out[i];
            delete[] delta_rec_23[i];
            delete[] delta_block_23[i];
            delete[] delta_rom_23[i];
        }

        return;
    }

    ////////////////////////////////////////////////////////////////////

    uchar new_stash_ptr[logNBytes];
    long_to_bytes(stash_ctr, new_stash_ptr, logNBytes);
    new_stash_ptr[0] = 1;

    uchar *stash_ptr_23[2];
    uchar *new_stash_ptr_23[2];
    for (uint i = 0; i < 2; i++) {
        stash_ptr_23[i] = new uchar[logNBytes];
        new_stash_ptr_23[i] = new_stash_ptr;
    }
    pos_map->access(addrPre_23, new_stash_ptr_23, false, stash_ptr_23);

    unsigned long mask2 = N - 1;
    unsigned long stash_addrPre_23[2];
    stash_addrPre_23[0] = bytes_to_long(stash_ptr_23[0], logNBytes) & mask2;
    stash_addrPre_23[1] = bytes_to_long(stash_ptr_23[1], logNBytes) & mask2;

    uchar *rom_block_23[2];
    uchar *stash_block_23[2];
    uchar *rom_fss_out[2];
    uchar *stash_fss_out[2];
    uchar *block_23[2];
    for (uint i = 0; i < 2; i++) {
        rom_block_23[i] = new uchar[DBytes];
        stash_block_23[i] = new uchar[DBytes];
        rom_fss_out[i] = new uchar[N];
        stash_fss_out[i] = new uchar[N];
        block_23[i] = new uchar[DBytes];
    }
    block_pir(addrPre_23, rom, N, rom_block_23, rom_fss_out);
    block_pir(stash_addrPre_23, stash, stash_ctr, stash_block_23,
              stash_fss_out);

    uchar indicator_23[2] = {stash_ptr_23[0][0], stash_ptr_23[1][0]};
    obliv_select(rom_block_23, stash_block_23, indicator_23, block_23);

    rec_pir(addrSuf_23, block_23, rec_23);
    uchar *delta_rec_23[2];
    uchar *delta_block_23[2];
    for (uint i = 0; i < 2; i++) {
        delta_rec_23[i] = new uchar[nextLogNBytes];
        if (isRead) {
            memset(delta_rec_23[i], 0, nextLogNBytes);
        } else {
            cal_xor(rec_23[i], new_rec_23[i], nextLogNBytes, delta_rec_23[i]);
        }
        delta_block_23[i] = new uchar[DBytes];
    }
    gen_delta_array(addrSuf_23, ttp, nextLogNBytes, delta_rec_23,
                    delta_block_23);

    update_wom(delta_block_23, rom_fss_out);
    append_stash(block_23, delta_block_23);

    for (uint i = 0; i < 2; i++) {
        delete[] stash_ptr_23[i];
        delete[] rom_block_23[i];
        delete[] stash_block_23[i];
        delete[] rom_fss_out[i];
        delete[] stash_fss_out[i];
        delete[] block_23[i];
        delete[] delta_rec_23[i];
        delete[] delta_block_23[i];
    }
}

void dpforam::print_metadata() {
    std::cout << "===================" << std::endl;
    std::cout << "Party: " << party << std::endl;
    std::cout << "Last level: " << isLast << std::endl;
    std::cout << "First level: " << isFirst << std::endl;
    std::cout << "tau: " << tau << std::endl;
    std::cout << "2^tau: " << ttp << std::endl;
    std::cout << "logN: " << logN << std::endl;
    std::cout << "N: " << N << std::endl;
    std::cout << "logNBytes: " << logNBytes << std::endl;
    std::cout << "nextLogN: " << nextLogN << std::endl;
    std::cout << "nextLogNBytes: " << nextLogNBytes << std::endl;
    std::cout << "DBytes: " << DBytes << std::endl;
    std::cout << "Stash counter: " << stash_ctr << std::endl;
    std::cout << "ROM: " << (rom != NULL) << std::endl;
    std::cout << "WOM: " << (wom != NULL) << std::endl;
    std::cout << "stash: " << (stash != NULL) << std::endl;
    std::cout << "posMap: " << (pos_map != NULL) << std::endl;
    std::cout << "===================\n"
              << std::endl;

    if (!isFirst) {
        pos_map->print_metadata();
    }
}

void dpforam::test(uint iter) {
    unsigned long party_wc = 0;
    unsigned long wc;

    print_metadata();

    bool isRead = false;
    unsigned long range = 1ul << (logN + tau);
    unsigned long addr_23[2] = {10, 10};
    uchar *rec_23[2];
    uchar *new_rec_23[2];
    for (uint i = 0; i < 2; i++) {
        rec_23[i] = new uchar[nextLogNBytes];
        new_rec_23[i] = new uchar[nextLogNBytes];
        memset(rec_23[i], 0, nextLogNBytes);
        memset(new_rec_23[i], 0, nextLogNBytes);
    }
    uchar rec_exp[nextLogNBytes];
    memset(rec_exp, 0, nextLogNBytes * sizeof(uchar));
    if (strcmp(party, "eddie") == 0) {
        addr_23[0] = rand_long(range);
        cons[0]->write_long(addr_23[0], false);
    } else if (strcmp(party, "debbie") == 0) {
        addr_23[1] = cons[1]->read_long();
    }

    for (uint t = 0; t < iter; t++) {
        if (strcmp(party, "eddie") == 0) {
            rnd->GenerateBlock(new_rec_23[0], nextLogNBytes);
            cons[0]->write(new_rec_23[0], nextLogNBytes, false);

            sync();
            wc = current_timestamp();
            access(addr_23, new_rec_23, isRead, rec_23);
            party_wc += current_timestamp() - wc;

            uchar rec_out[nextLogNBytes];
            cons[0]->read(rec_out, nextLogNBytes);
            cal_xor(rec_out, rec_23[0], nextLogNBytes, rec_out);
            cal_xor(rec_out, rec_23[1], nextLogNBytes, rec_out);

            if (memcmp(rec_exp, rec_out, nextLogNBytes) == 0) {
                std::cout << "addr=" << addr_23[0] << ", t=" << t << ": Pass"
                          << std::endl;
            } else {
                std::cerr << "addr=" << addr_23[0] << ", t=" << t
                          << ": Fail !!!" << std::endl;
            }

            memcpy(rec_exp, new_rec_23[0], nextLogNBytes);
        } else if (strcmp(party, "debbie") == 0) {
            cons[1]->read(new_rec_23[1], nextLogNBytes);

            sync();
            wc = current_timestamp();
            access(addr_23, new_rec_23, isRead, rec_23);
            party_wc += current_timestamp() - wc;

            cons[1]->write(rec_23[0], nextLogNBytes, false);
        } else if (strcmp(party, "charlie") == 0) {
            sync();
            wc = current_timestamp();
            access(addr_23, new_rec_23, isRead, rec_23);
            party_wc += current_timestamp() - wc;
        } else {
            std::cout << "Incorrect party: " << party << std::endl;
        }
    }

    for (uint i = 0; i < 2; i++) {
        delete[] rec_23[i];
        delete[] new_rec_23[i];
    }

    unsigned long party_band = bandwidth();
    cons[0]->write_long(party_band, false);
    cons[1]->write_long(party_band, false);
    unsigned long total_band = party_band;
    total_band += (unsigned long)cons[0]->read_long();
    total_band += (unsigned long)cons[1]->read_long();

    cons[0]->write_long(party_wc, false);
    cons[1]->write_long(party_wc, false);
    unsigned long max_wc = party_wc;
    max_wc = std::max(max_wc, (unsigned long)cons[0]->read_long());
    max_wc = std::max(max_wc, (unsigned long)cons[1]->read_long());

    std::cout << std::endl;
    std::cout << "Party Bandwidth(byte): " << (party_band / iter) << std::endl;
    std::cout << "Party Wallclock(microsec): " << (party_wc / iter)
              << std::endl;
    std::cout << "Total Bandwidth(byte): " << (total_band / iter) << std::endl;
    std::cout << "Max Wallclock(microsec): " << (max_wc / iter) << std::endl;
    std::cout << std::endl;
}
