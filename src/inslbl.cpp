#include <iostream>

#include "inslbl.h"
#include "util.h"

inslbl::inslbl(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) :
		protocol(party, cons, rnd, prgs) {
}

void xor_rot(const char* in, int r, int ttp, int lBytes, char* out) {
	for (int i = 0; i < ttp; i++) {
		for (int j = 0; j < lBytes; j++) {
			out[i * lBytes + j] = in[(i ^ r) * lBytes + j];
		}
	}
}

void inslbl::runE(int dN1, const char* L1, int ttp, int lBytes) {
	// offline
	int len = ttp * lBytes;
	char p[len];
	char a[len];
	char b[len];
	prgs[0].GenerateBlock((unsigned char*) p, len);
	prgs[0].GenerateBlock((unsigned char*) a, len);
	prgs[0].GenerateBlock((unsigned char*) b, len);
	int v = prgs[0].GenerateWord32(0, ttp - 1);
	int w = prgs[0].GenerateWord32(0, ttp - 1);
	int alpha1 = rnd->GenerateWord32(0, ttp - 1);
	int u1 = alpha1 ^ v;
	char pstar[len];
	xor_rot(a, u1, ttp, lBytes, pstar);
	cal_xor(p, pstar, len, pstar);
	cons[1]->write_int(u1);
	cons[1]->write(pstar, len);

	// online
	int m = dN1 ^ alpha1;
	cons[0]->write_int(m);
	m = cons[0]->read_int();
	int beta1 = m ^ dN1;
	int index = beta1 ^ w;
	for (int i = 0; i < lBytes; i++) {
		b[index * lBytes + i] = (char) (b[index * lBytes + i] ^ L1[i]);
	}
	cons[1]->write(b, len);
}

void inslbl::runD(int dN2, const char* L2, int ttp, int lBytes, char* z2) {
	// offline
	int len = ttp * lBytes;
	char p[len];
	char a[len];
	char b[len];
	prgs[1].GenerateBlock((unsigned char*) p, len);
	prgs[1].GenerateBlock((unsigned char*) a, len);
	prgs[1].GenerateBlock((unsigned char*) b, len);
	int v = prgs[1].GenerateWord32(0, ttp - 1);
	int w = prgs[1].GenerateWord32(0, ttp - 1);
	int beta2 = rnd->GenerateWord32(0, ttp - 1);
	int u2 = beta2 ^ w;
	xor_rot(b, u2, ttp, lBytes, z2);
	cal_xor(p, z2, len, z2);
	cons[0]->write_int(u2);

	// online
	int m = beta2 ^ dN2;
	cons[1]->write_int(m);
	m = cons[1]->read_int();
	int alpha2 = m ^ dN2;
	int index = alpha2 ^ v;
	for (int i = 0; i < lBytes; i++) {
		a[index * lBytes + i] = (char) (a[index * lBytes + i] ^ L2[i]);
	}
	cons[0]->write(a, len);
}

void inslbl::runC(int ttp, int lBytes, char* pstar) {
	// offline
	int len = ttp * lBytes;
	int u1 = cons[0]->read_int();
	cons[0]->read(pstar, len);
	int u2 = cons[1]->read_int();

	// online
	char s1[len];
	char s2[len];
	char s1p[len];
	char s2p[len];
	cons[0]->read(s1, len);
	cons[1]->read(s2, len);
	xor_rot(s2, u1, ttp, lBytes, s2p);
	xor_rot(s1, u2, ttp, lBytes, s1p);
	cal_xor(pstar, s1p, len, pstar);
	cal_xor(pstar, s2p, len, pstar);
}

void inslbl::test() {
	for (int test = 0; test < 100; test++) {
		int ttp = 16;
		int lBytes = 20;
		int len = ttp * lBytes;
		int dN1 = rnd->GenerateWord32(0, ttp - 1);
		int dN2 = rnd->GenerateWord32(0, ttp - 1);
		char L1[lBytes];
		char L2[lBytes];
		rnd->GenerateBlock((unsigned char*) L1, lBytes);
		rnd->GenerateBlock((unsigned char*) L2, lBytes);
		char z2[len];
		char pstar[len];

		if (strcmp(party, "eddie") == 0) {
			runE(dN1, L1, ttp, lBytes);
			cons[1]->read(pstar, len);
			dN2 = cons[0]->read_int();
			cons[0]->read(L2, lBytes);
			cons[0]->read(z2, len);
			int dN = dN1 ^ dN2;
			char L[lBytes];
			cal_xor(L1, L2, lBytes, L);
			char out[len];
			cal_xor(z2, pstar, len, out);
			char zero[lBytes] = { 0 };
			bool pass = true;
			for (int i = 0; i < ttp; i++) {
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
