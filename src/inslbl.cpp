#include "inslbl.h"

#include <iostream>

#include "util.h"

inslbl::inslbl(const char *party, connection *cons[2],
               CryptoPP::AutoSeededRandomPool *rnd,
               CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption *prgs) : protocol(party, cons, rnd, prgs) {
}

void xor_perm(const uchar *in, uint r, uint ttp, uint lBytes, uchar *out) {
    for (uint i = 0; i < ttp; i++) {
        memcpy(out + i * lBytes, in + (i ^ r) * lBytes, lBytes);
    }
}

void inslbl::runE(uint dN1, const uchar *L1, uint ttp, uint lBytes) {
    // offline
    uint len = ttp * lBytes;
    uchar p[len];
    uchar a[len];
    uchar b[len];
    prgs[0].GenerateBlock(p, len);
    prgs[0].GenerateBlock(a, len);
    prgs[0].GenerateBlock(b, len);
    uint v = prgs[0].GenerateWord32(0, ttp - 1);
    uint w = prgs[0].GenerateWord32(0, ttp - 1);
    uint alpha1 = rnd->GenerateWord32(0, ttp - 1);
    uint u1 = alpha1 ^ v;
    uchar pstar[len];
    xor_perm(a, u1, ttp, lBytes, pstar);
    cal_xor(p, pstar, len, pstar);
    cons[1]->write_int(u1);
    cons[1]->write(pstar, len);

    // online
    uint m = dN1 ^ alpha1;
    cons[0]->write_int(m);
    m = cons[0]->read_int();
    uint beta1 = m ^ dN1;
    uint index = beta1 ^ w;
    cal_xor(b + index * lBytes, L1, lBytes, b + index * lBytes);
    cons[1]->write(b, len);
}

void inslbl::runD(uint dN2, const uchar *L2, uint ttp, uint lBytes, uchar *z2) {
    // offline
    uint len = ttp * lBytes;
    uchar p[len];
    uchar a[len];
    uchar b[len];
    prgs[1].GenerateBlock(p, len);
    prgs[1].GenerateBlock(a, len);
    prgs[1].GenerateBlock(b, len);
    uint v = prgs[1].GenerateWord32(0, ttp - 1);
    uint w = prgs[1].GenerateWord32(0, ttp - 1);
    uint beta2 = rnd->GenerateWord32(0, ttp - 1);
    uint u2 = beta2 ^ w;
    xor_perm(b, u2, ttp, lBytes, z2);
    cal_xor(p, z2, len, z2);
    cons[0]->write_int(u2);

    // online
    uint m = beta2 ^ dN2;
    cons[1]->write_int(m);
    m = cons[1]->read_int();
    uint alpha2 = m ^ dN2;
    uint index = alpha2 ^ v;
    cal_xor(a + index * lBytes, L2, lBytes, a + index * lBytes);
    cons[0]->write(a, len);
}

void inslbl::runC(uint ttp, uint lBytes, uchar *pstar) {
    // offline
    uint len = ttp * lBytes;
    uint u1 = cons[0]->read_int();
    cons[0]->read(pstar, len);
    uint u2 = cons[1]->read_int();

    // online
    uchar s1[len];
    uchar s2[len];
    uchar s1p[len];
    uchar s2p[len];
    cons[0]->read(s1, len);
    cons[1]->read(s2, len);
    xor_perm(s2, u1, ttp, lBytes, s2p);
    xor_perm(s1, u2, ttp, lBytes, s1p);
    cal_xor(pstar, s1p, len, pstar);
    cal_xor(pstar, s2p, len, pstar);
}

void inslbl::test(uint iter) {
    for (uint test = 0; test < iter; test++) {
        uint ttp = 256;
        uint lBytes = 16;
        uint len = ttp * lBytes;
        uint dN1 = rnd->GenerateWord32(0, ttp - 1);
        uint dN2 = rnd->GenerateWord32(0, ttp - 1);
        uchar L1[lBytes];
        uchar L2[lBytes];
        rnd->GenerateBlock(L1, lBytes);
        rnd->GenerateBlock(L2, lBytes);
        uchar z2[len];
        uchar pstar[len];

        if (strcmp(party, "eddie") == 0) {
            runE(dN1, L1, ttp, lBytes);
            cons[1]->read(pstar, len);
            dN2 = cons[0]->read_int();
            cons[0]->read(L2, lBytes);
            cons[0]->read(z2, len);
            uint dN = dN1 ^ dN2;
            uchar L[lBytes];
            cal_xor(L1, L2, lBytes, L);
            uchar out[len];
            cal_xor(z2, pstar, len, out);
            uchar zero[lBytes];
            memset(zero, 0, lBytes * sizeof(uchar));
            bool pass = true;
            for (uint i = 0; i < ttp; i++) {
                if (i == dN) {
                    if (memcmp(L, out + i * lBytes, lBytes) != 0) {
                        pass = false;
                        break;
                    }
                } else {
                    if (memcmp(zero, out + i * lBytes, lBytes) != 0) {
                        pass = false;
                        break;
                    }
                }
            }
            if (pass) {
                std::cout << "InsLbl passed: " << test << std::endl;
            } else {
                std::cerr << "!!!!! InsLbl failed: " << test << std::endl;
            }
        } else if (strcmp(party, "debbie") == 0) {
            runD(dN2, L2, ttp, lBytes, z2);
            cons[1]->write_int(dN2);
            cons[1]->write(L2, lBytes);
            cons[1]->write(z2, len);
        } else if (strcmp(party, "charlie") == 0) {
            runC(ttp, lBytes, pstar);
            cons[0]->write(pstar, len);
        } else {
            std::cout << "Incorrect party: " << party << std::endl;
        }
    }
}
