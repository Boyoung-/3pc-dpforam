#include "protocol.h"

protocol::protocol(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs) {
	this->party = party;
	this->cons = cons;
	this->rnd = rnd;
	this->prgs = prgs;
}

protocol::~protocol() {
}

void protocol::sync() {
	uchar z = 0;
	cons[0]->write(&z, 1);
	cons[1]->write(&z, 1);
	cons[0]->read(&z, 1);
	cons[1]->read(&z, 1);
}

void protocol::test() {
}
