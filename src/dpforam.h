#ifndef DPFORAM_H_
#define DPFORAM_H_

#include "protocol.h"

class dpforam: public protocol {
private:
	char*** rom;
	char** wom;
	char*** stash;
	dpforam* pos_map;
	long stash_ctr;
public:
	int logN;
	int logNBytes;
	int nextLogN;
	int nextLogNBytes;
	int tau;
	int ttp;
	int DBytes;
	long N;
	bool isFirst;
	bool isLast;

private:
	void init();
	void init_ctr();
	void set_zero(char** mem);
	void init_mem(char** &mem);
	void delete_mem(char** mem);
	int cal_last_tau(int DBytes);
public:
	dpforam(const char* party, connection* cons[2],
			CryptoPP::AutoSeededRandomPool* rnd,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs, int tau,
			int logN, int DBytes, bool isLast);
	~dpforam();
	void print_metadata();
	void test();
};

#endif /* DPFORAM_H_ */
