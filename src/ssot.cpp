#include <cryptopp/osrng.h>
#include <iostream>

#include "ssot.h"
#include "util.h"

using namespace std;

ssot::ssot(const char* party, connection* cons[2],
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) :
		protocol(party, cons, prgs) {

}

char* ssot::runE(int b1, char* v01[2], int mBytes) {
	char y01[2][mBytes];
	prgs[0].GenerateBlock((unsigned char*) y01[0], mBytes);
	prgs[0].GenerateBlock((unsigned char*) y01[1], mBytes);
	int e = prgs[0].GenerateBit();
	char* x;
	cons[0]->read(x);

	delete[] x;
	return NULL;
}

void ssot::runD(int mBytes) {
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
	char* x = get_xor(x01[e], delta, mBytes);
	char* y = get_xor(y01[c], delta, mBytes);
	cons[0]->write(y, mBytes);
	cons[1]->write(x, mBytes);

	delete[] x;
	delete[] y;
}

char* ssot::runC(int b0, char* u01[2], int mBytes) {
	char x01[2][mBytes];
	prgs[1].GenerateBlock((unsigned char*) x01[0], mBytes);
	prgs[1].GenerateBlock((unsigned char*) x01[1], mBytes);
	int c = prgs[1].GenerateBit();
	char* y;
	cons[1]->read(y);

	delete[] y;
	return NULL;
}

void ssot::test() {
	int mBytes = 10;
	char* u01[2] = { new char[mBytes], new char[mBytes] };
	char* v01[2] = { new char[mBytes], new char[mBytes] };
	int b0 = 0;
	int b1 = 1;

	if (strcmp(party, "eddie") == 0) {
		runE(b1, v01, mBytes);
	} else if (strcmp(party, "debbie") == 0) {
		runD(mBytes);
	} else if (strcmp(party, "charlie") == 0) {
		runC(b0, u01, mBytes);
	} else {
		std::cout << "Incorrect party: " << party << std::endl;
	}

	std::cout << "DONE" << std::endl;

	for (int i = 0; i < 2; i++) {
		delete[] u01[i];
		delete[] v01[i];
	}
}
