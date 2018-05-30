#ifndef SSOT_H_
#define SSOT_H_

#include "protocol.h"

class ssot: public protocol {
public:
	ssot(connection* cons[],
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs[]);

	void test();
};

#endif /* SSOT_H_ */
