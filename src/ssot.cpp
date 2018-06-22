#include <iostream>

#include "ssot.h"
#include "util.h"

ssot::ssot(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) :
		protocol(party, cons, rnd, prgs) {
}

void ssot::runE(uint b1, const uchar* const v01[2], uint mBytes, uchar* p1) {
	// offline
	uchar y01[2][mBytes];
	prgs[0].GenerateBlock(y01[0], mBytes);
	prgs[0].GenerateBlock(y01[1], mBytes);
	uint e = prgs[0].GenerateBit();
	uchar x[mBytes];
	cons[0]->read(x, mBytes);

	// online
	uint t = b1 ^ e;
	cons[1]->write_int(t);
	uint s = cons[1]->read_int();
	uchar v01_p[2][mBytes];
	cal_xor(v01[b1], y01[s], mBytes, v01_p[0]);
	cal_xor(v01[1 - b1], y01[1 - s], mBytes, v01_p[1]);
	cons[1]->write(v01_p[0], mBytes);
	cons[1]->write(v01_p[1], mBytes);
	uchar u01_p[2][mBytes];
	cons[1]->read(u01_p[0], mBytes);
	cons[1]->read(u01_p[1], mBytes);
	cal_xor(u01_p[b1], x, mBytes, p1);
}

void ssot::runD(uint mBytes) {
	// offline
	uchar x01[2][mBytes];
	prgs[0].GenerateBlock(x01[0], mBytes);
	prgs[0].GenerateBlock(x01[1], mBytes);
	uchar y01[2][mBytes];
	prgs[1].GenerateBlock(y01[0], mBytes);
	prgs[1].GenerateBlock(y01[1], mBytes);
	uchar delta[mBytes];
	rnd->GenerateBlock(delta, mBytes);
	uint c = prgs[0].GenerateBit();
	uint e = prgs[1].GenerateBit();
	uchar x[mBytes];
	cal_xor(x01[e], delta, mBytes, x);
	uchar y[mBytes];
	cal_xor(y01[c], delta, mBytes, y);
	cons[0]->write(y, mBytes);
	cons[1]->write(x, mBytes);
}

void ssot::runC(uint b0, const uchar* const u01[2], uint mBytes, uchar* p0) {
	// offline
	uchar x01[2][mBytes];
	prgs[1].GenerateBlock(x01[0], mBytes);
	prgs[1].GenerateBlock(x01[1], mBytes);
	uint c = prgs[1].GenerateBit();
	uchar y[mBytes];
	cons[1]->read(y, mBytes);

	// online
	uint s = b0 ^ c;
	cons[0]->write_int(s);
	uint t = cons[0]->read_int();
	uchar u01_p[2][mBytes];
	cal_xor(u01[b0], x01[t], mBytes, u01_p[0]);
	cal_xor(u01[1 - b0], x01[1 - t], mBytes, u01_p[1]);
	cons[0]->write(u01_p[0], mBytes);
	cons[0]->write(u01_p[1], mBytes);
	uchar v01_p[2][mBytes];
	cons[0]->read(v01_p[0], mBytes);
	cons[0]->read(v01_p[1], mBytes);
	cal_xor(v01_p[b0], y, mBytes, p0);
}

void ssot::test(uint iter) {
	for (uint test = 0; test < iter; test++) {
		uint mBytes = 50;
		uint b0 = rnd->GenerateBit();
		uint b1 = rnd->GenerateBit();
		uchar* u01[2] = { new uchar[mBytes], new uchar[mBytes] };
		uchar* v01[2] = { new uchar[mBytes], new uchar[mBytes] };
		for (uint i = 0; i < 2; i++) {
			rnd->GenerateBlock(u01[i], mBytes);
			rnd->GenerateBlock(v01[i], mBytes);
		}
		uchar p0[mBytes];
		uchar p1[mBytes];

		if (strcmp(party, "eddie") == 0) {
			runE(b1, v01, mBytes, p1);
			b0 = cons[1]->read_int();
			cons[1]->read(p0, mBytes);
			cons[1]->read(u01[0], mBytes);
			cons[1]->read(u01[1], mBytes);
			uint b = b0 ^ b1;
			uchar output[mBytes];
			uchar expected[mBytes];
			cal_xor(u01[b], v01[b], mBytes, expected);
			cal_xor(p0, p1, mBytes, output);
			if (memcmp(output, expected, mBytes) == 0) {
				std::cout << "SSOT passed: " << test << std::endl;
			} else {
				std::cerr << "!!!!! SSOT failed: " << test << std::endl;
			}

		} else if (strcmp(party, "debbie") == 0) {
			runD(mBytes);

		} else if (strcmp(party, "charlie") == 0) {
			runC(b0, u01, mBytes, p0);
			cons[0]->write_int(b0);
			cons[0]->write(p0, mBytes);
			cons[0]->write(u01[0], mBytes);
			cons[0]->write(u01[1], mBytes);

		} else {
			std::cout << "Incorrect party: " << party << std::endl;
		}

		for (uint i = 0; i < 2; i++) {
			delete[] u01[i];
			delete[] v01[i];
		}
	}
}
