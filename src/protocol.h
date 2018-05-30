#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include "connection.h"

class protocol {
private:
	connection* cons;
	CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs;
public:
	const char* party;

	protocol(const char* party, connection* cons,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) {
		this->party = party;
		this->cons = cons;
		this->prgs = prgs;
	}
};

#endif /* PROTOCOL_H_ */
