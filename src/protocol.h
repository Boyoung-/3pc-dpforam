#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

#include "connection.h"

// TODO: bandwidth
class protocol {
protected:
	connection** cons;
	CryptoPP::AutoSeededRandomPool* rnd;
	CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs;
public:
	const char* party;

	protocol(const char* party, connection* cons[2],
			CryptoPP::AutoSeededRandomPool* rnd,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);
	virtual ~protocol();
	void sync();
	virtual void test();
};

#endif /* PROTOCOL_H_ */
