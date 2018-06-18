#include <algorithm>
#include <iostream>

#include "dpforam.h"
#include "inslbl.h"
#include "util.h"

fss1bit dpforam::fss;

void dpforam::init() {
	init_ctr();

	set_zero(rom[0]);
	set_zero(rom[1]);
	set_zero(wom);
	if (stash != NULL) {
		set_zero(stash[0]);
		set_zero(stash[1]);
	}

	if (!isFirst) {
		pos_map->init();
	}
}

void dpforam::init_ctr() {
	stash_ctr = 1;
}

void dpforam::set_zero(uchar** mem) {
	if (mem == NULL) {
		return;
	}

	for (ulong i = 0; i < N; i++) {
		memset(mem[i], 0, DBytes);
	}
}

void dpforam::init_mem(uchar** &mem) {
	mem = new uchar*[N];
	for (ulong i = 0; i < N; i++) {
		mem[i] = new uchar[DBytes];
	}
}

void dpforam::delete_mem(uchar** mem) {
	for (ulong i = 0; i < N; i++) {
		delete[] mem[i];
	}
	delete[] mem;
}

uint dpforam::cal_last_tau(uint DBytes) {
	if (DBytes < 2) {
		return 4;
	} else if (DBytes < 4) {
		return 3;
	} else if (DBytes < 8) {
		return 2;
	} else if (DBytes < 16) {
		return 1;
	} else {
		return 0;
	}
}

void dpforam::block_pir(const ulong addr_with_flag_23[2],
		const uchar* const * const mem_23[2], uchar* block_23[2],
		uchar* fss_out[2]) {
	ulong mask = N - 1;
	ulong addr_23[2];
	addr_23[0] = addr_with_flag_23[0] & mask;
	addr_23[1] = addr_with_flag_23[1] & mask;

	uchar* keys[2];
	uint keyBytes = fss.gen(addr_23[0] ^ addr_23[1], logN, keys);
	cons[0]->write(keys[0], keyBytes);
	cons[1]->write(keys[1], keyBytes);
	cons[0]->read(keys[1], keyBytes);
	cons[1]->read(keys[0], keyBytes);

	memset(block_23[0], 0, DBytes);
	for (uint i = 0; i < 2; i++) {
		fss.eval_all_with_shift(keys[i], logN, addr_23[i], fss_out[i]);
		for (ulong j = 0; j < N; j++) {
			if (fss_out[i][j] == 1) {
				cal_xor(block_23[0], mem_23[i][j], DBytes, block_23[0]);
			}
		}
	}

	cons[0]->write(block_23[0], DBytes);
	cons[1]->read(block_23[1], DBytes);

	delete[] keys[0];
	delete[] keys[1];
}

void dpforam::rec_pir(const uint idx_23[2], const uchar* const block_23[2],
		uchar* rec_23[2]) {
	uchar* keys[2];
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
			if (fss_out[j ^ idx_23[i]] == 1) {
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

void dpforam::gen_delta_array(const uint idx_23[2], uint numChunk, uint chunkBytes,
		const uchar* const delta_23[2], uchar* delta_array_23[2]) {
	uint arrayBytes = numChunk * chunkBytes;
	inslbl il(party, cons, rnd, prgs);
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

dpforam::dpforam(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs, uint tau, uint logN,
		uint DBytes, bool isLast) :
		protocol(party, cons, rnd, prgs) {
	this->isLast = isLast;
	this->tau = isLast ? cal_last_tau(DBytes) : tau;
	this->logN = isLast ? (logN - this->tau) : logN;
	ttp = 1 << this->tau;
	logNBytes = (this->logN + 7) / 8 + 1;
	nextLogN = isLast ? 0 : logN + tau;
	nextLogNBytes = isLast ? DBytes : (nextLogN + 7) / 8 + 1;
	this->DBytes = nextLogNBytes * ttp;
	N = 1L << this->logN;
	isFirst = this->logN - tau < tau;

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

bool dpforam::check_sharing(const uchar* const share_23[2], uint len,
		const uchar* expect) {
	uchar tmp[len];
	cons[0]->write(share_23[0], len);
	cons[1]->read(tmp, len);
	if (memcmp(tmp, share_23[1], len) != 0) {
		std::cout << "!!!" << std::endl;
		return false;
	}
	cons[1]->write(share_23[0], len);
	cons[0]->read(tmp, len);
	cal_xor(tmp, share_23[0], len, tmp);
	cal_xor(tmp, share_23[1], len, tmp);
	if (memcmp(tmp, expect, len) != 0) {
		for (uint i = 0; i < len; i++) {
			std::cout << (uint) tmp[i] << " ";
		}
		std::cout << std::endl;
	}
	return memcmp(tmp, expect, len) == 0;
}

void dpforam::access(const ulong addr_23[2], const uchar* const newRec_23[2],
		bool isRead, uchar* rec_23[2]) {
	uint mask = ttp - 1;
	ulong addrPre_23[2];
	uint addrSuf_23[2];
	for (uint i = 0; i < 2; i++) {
		addrPre_23[i] = addr_23[i] >> tau;
		addrSuf_23[i] = (uint) addr_23[i] & mask;
	}

	uchar* block_23[2];
	uchar* pir_out[2];
	uchar* delta_rec_23[2];
	uchar* delta_block_23[2];
	uchar* delta_rom_23[2];
	for (uint i = 0; i < 2; i++) {
		block_23[i] = new uchar[DBytes];
		pir_out[i] = new uchar[N];
		delta_rec_23[i] = new uchar[nextLogNBytes];
		delta_block_23[i] = new uchar[DBytes];
		delta_rom_23[i] = new uchar[N * DBytes];
	}
	block_pir(addrPre_23, rom, block_23, pir_out);
	rec_pir(addrSuf_23, block_23, rec_23);

	cal_xor(rec_23[0], newRec_23[0], nextLogNBytes, delta_rec_23[0]);
	cal_xor(rec_23[1], newRec_23[1], nextLogNBytes, delta_rec_23[1]);

	//char zero1[nextLogNBytes] = {0};
	//std::cout << "check point 1: " << check_sharing(delta_rec_23, nextLogNBytes, zero1) << std::endl;

	gen_delta_array(addrSuf_23, ttp, nextLogNBytes, delta_rec_23,
			delta_block_23);

	//char zero2[DBytes] = {0};
	//std::cout << "check point 2: " << check_sharing(delta_block_23, DBytes, zero2) << std::endl;

	uint int_addrPre_23[2] = { (uint) addrPre_23[0], (uint) addrPre_23[1] };
	gen_delta_array(int_addrPre_23, (uint) N, DBytes, delta_block_23,
			delta_rom_23);

	//char zero3[N*DBytes] = {0};
	//std::cout << "check point 3: " << check_sharing(delta_rom_23, N*DBytes, zero3) << std::endl;

	for (uint i = 0; i < 2; i++) {
		for (ulong j = 0; j < N; j++) {
			cal_xor(rom[i][j], delta_rom_23[i] + j * DBytes, DBytes, rom[i][j]);
		}
	}

	for (uint i = 0; i < 2; i++) {
		delete[] block_23[i];
		delete[] pir_out[i];
		delete[] delta_rec_23[i];
		delete[] delta_block_23[i];
		delete[] delta_rom_23[i];
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
	std::cout << "===================\n" << std::endl;

	if (!isFirst) {
		pos_map->print_metadata();
	}
}

void dpforam::test() {
	print_metadata();

	ulong addr_23[2] = { 0, 0 };
	uchar* rec_23[2];
	uchar* newRec_23[2];
	for (uint i = 0; i < 2; i++) {
		rec_23[i] = new uchar[nextLogNBytes];
		newRec_23[i] = new uchar[nextLogNBytes];
		memset(rec_23[i], 0, nextLogNBytes);
		memset(newRec_23[i], 0, nextLogNBytes);
	}

	uchar rec_exp[nextLogNBytes] = { 0 };
	for (uint t = 0; t < 10; t++) {
		if (strcmp(party, "eddie") == 0) {
			rnd->GenerateBlock(newRec_23[0], nextLogNBytes);
			cons[0]->write(newRec_23[0], nextLogNBytes);

			access(addr_23, newRec_23, true, rec_23);

			uchar rec_out[nextLogNBytes];
			cons[0]->read(rec_out, nextLogNBytes);
			cal_xor(rec_out, rec_23[0], nextLogNBytes, rec_out);
			cal_xor(rec_out, rec_23[1], nextLogNBytes, rec_out);

			if (memcmp(rec_exp, rec_out, nextLogNBytes) == 0) {
				std::cout << "t=" << t << ": Pass" << std::endl;
			} else {
				std::cerr << "t=" << t << ": Fail" << std::endl;
			}

			memcpy(rec_exp, newRec_23[0], nextLogNBytes);

		} else if (strcmp(party, "debbie") == 0) {
			cons[1]->read(newRec_23[1], nextLogNBytes);
			access(addr_23, newRec_23, true, rec_23);
			cons[1]->write(rec_23[0], nextLogNBytes);

		} else if (strcmp(party, "charlie") == 0) {
			access(addr_23, newRec_23, true, rec_23);

		} else {
		}
	}

	for (uint i = 0; i < 2; i++) {
		delete[] rec_23[i];
		delete[] newRec_23[i];
	}
}
