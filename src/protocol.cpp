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
	cons[0]->write_int(0);
	cons[1]->write_int(0);
	cons[0]->read_int();
	cons[1]->read_int();
}

void protocol::test() {
}
