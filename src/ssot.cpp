#include "ssot.h"

ssot::ssot(const char* party, connection* cons,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) :
		protocol(party, cons, prgs) {
}

#include <iostream>
void ssot::test() {
	std::cout << party << std::endl;
}
