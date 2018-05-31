#include <cryptopp/osrng.h>
#include <iostream>

#include "ssot.h"
#include "util.h"

ssot::ssot(const char* party, connection* cons[2],
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) :
		protocol(party, cons, prgs) {

}

void ssot::runE(int b1, const char* v01[2], int mBytes, char* p1) {
	// offline
	char y01[2][mBytes];
	prgs[0].GenerateBlock((unsigned char*) y01[0], mBytes);
	prgs[0].GenerateBlock((unsigned char*) y01[1], mBytes);
	int e = prgs[0].GenerateBit();
	char x[mBytes];
	cons[0]->read(x, mBytes);

	// online
	int t = b1 ^ e;
	cons[1]->write_int(t);
	int s = cons[1]->read_int();

	char v01_p[2][mBytes];
	cal_xor(v01[b1], y01[s], v01_p[0], mBytes);
	cal_xor(v01[1 - b1], y01[1 - s], v01_p[1], mBytes);

	cons[1]->write(v01_p[0], mBytes);
	cons[1]->write(v01_p[1], mBytes);

	char u01_p[2][mBytes];
	cons[1]->read(u01_p[0], mBytes);
	cons[1]->read(u01_p[1], mBytes);

	cal_xor(u01_p[b1], x, p1, mBytes);
}

void ssot::runD(int mBytes) {
	// offline
	char x01[2][mBytes];
	prgs[0].GenerateBlock((unsigned char*) x01[0], mBytes);
	prgs[0].GenerateBlock((unsigned char*) x01[1], mBytes);
	char y01[2][mBytes];
	prgs[1].GenerateBlock((unsigned char*) y01[0], mBytes);
	prgs[1].GenerateBlock((unsigned char*) y01[1], mBytes);
	char delta[mBytes];
	CryptoPP::OS_GenerateRandomBlock(true, (unsigned char*) delta, mBytes);
	int c = prgs[0].GenerateBit();
	int e = prgs[1].GenerateBit();
	char x[mBytes];
	cal_xor(x01[e], delta, x, mBytes);
	char y[mBytes];
	cal_xor(y01[c], delta, y, mBytes);
	cons[0]->write(y, mBytes);
	cons[1]->write(x, mBytes);
}

void ssot::runC(int b0, const char* u01[2], int mBytes, char* p0) {
	// offline
	char x01[2][mBytes];
	prgs[1].GenerateBlock((unsigned char*) x01[0], mBytes);
	prgs[1].GenerateBlock((unsigned char*) x01[1], mBytes);
	int c = prgs[1].GenerateBit();
	char y[mBytes];
	cons[1]->read(y, mBytes);

	// online
	int s = b0 ^ c;
	cons[0]->write_int(s);
	int t = cons[0]->read_int();

	char u01_p[2][mBytes];
	cal_xor(u01[b0], x01[t], u01_p[0], mBytes);
	cal_xor(u01[1 - b0], x01[1 - t], u01_p[1], mBytes);
	cons[0]->write(u01_p[0], mBytes);
	cons[0]->write(u01_p[1], mBytes);
	char v01_p[2][mBytes];
	cons[0]->read(v01_p[0], mBytes);
	cons[0]->read(v01_p[1], mBytes);
	cal_xor(v01_p[b0], y, p0, mBytes);
}

void ssot::test() {
	int mBytes = 10;
	const char* u01[2] = { new char[mBytes], new char[mBytes] };
	const char* v01[2] = { new char[mBytes], new char[mBytes] };
	int b0 = 0;
	int b1 = 1;
	char p0[mBytes];
	char p1[mBytes];

	if (strcmp(party, "eddie") == 0) {
		runE(b1, v01, mBytes, p1);
	} else if (strcmp(party, "debbie") == 0) {
		runD(mBytes);
	} else if (strcmp(party, "charlie") == 0) {
		runC(b0, u01, mBytes, p0);
	} else {
		std::cout << "Incorrect party: " << party << std::endl;
	}

	std::cout << "DONE" << std::endl;

	for (int i = 0; i < 2; i++) {
		delete[] u01[i];
		delete[] v01[i];
	}
}
