#ifndef SSOT_H_
#define SSOT_H_

#include "protocol.h"

class ssot: public protocol {
public:
	ssot(const char* party, connection* cons,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);

	void test();
};

#endif /* SSOT_H_ */
