#ifndef SSOT_H_
#define SSOT_H_

#include "protocol.h"

class ssot: public protocol {
public:
	ssot(const char* party, connection* cons,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);

	char* runE(int b1, char* v01[2], int mBytes);

	void runD(int mBytes);

	char* runC(int b0, char* u01[2], int mBytes);

	void test();
};

#endif /* SSOT_H_ */
