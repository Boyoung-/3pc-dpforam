#include "ssot.h"

ssot::ssot(connection* cons[],
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs[]) :
		protocol(cons, prgs) {
}

void ssot::test() {
}
