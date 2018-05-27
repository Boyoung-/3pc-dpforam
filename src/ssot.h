#ifndef SSOT_H_
#define SSOT_H_

#include <cryptopp/randpool.h>

#include "connection.h"

class ssot {
private:
	connection* cons[2];
	CryptoPP::RandomPool* rnds[2];
};

#endif /* SSOT_H_ */
