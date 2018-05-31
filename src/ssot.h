#ifndef SSOT_H_
#define SSOT_H_

#include "protocol.h"

class ssot: public protocol {
public:
	ssot(const char* party, connection* cons[2],
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);

	void runE(int b1, const char* v01[2], int mBytes, char* p1);

	void runD(int mBytes);

	void runC(int b0, const char* u01[2], int mBytes, char* p0);

	void test();
};

#endif /* SSOT_H_ */
