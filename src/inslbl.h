#ifndef INSLBL_H_
#define INSLBL_H_

#include "protocol.h"

class inslbl: public protocol {
public:
	inslbl(const char* party, connection* cons[2],
			CryptoPP::AutoSeededRandomPool* rnd,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);

	void runE(int dN1, const char* L1, int ttp, int lBytes);
	void runD(int dN2, const char* L2, int ttp, int lBytes, char* z2);
	void runC(int ttp, int lBytes, char* pstar);
	void test();
};

#endif /* INSLBL_H_ */
