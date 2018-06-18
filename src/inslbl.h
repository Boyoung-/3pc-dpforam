#ifndef INSLBL_H_
#define INSLBL_H_

#include "protocol.h"

class inslbl: public protocol {
public:
	inslbl(const char* party, connection* cons[2],
			CryptoPP::AutoSeededRandomPool* rnd,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);
	void runE(uint dN1, const uchar* L1, uint ttp, uint lBytes);
	void runD(uint dN2, const uchar* L2, uint ttp, uint lBytes, uchar* z2);
	void runC(uint ttp, uint lBytes, uchar* pstar);
	void test();
};

#endif /* INSLBL_H_ */
