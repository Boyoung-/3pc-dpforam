#ifndef DPFORAM_H_
#define DPFORAM_H_

#include "fss.h"
#include "protocol.h"

class dpforam: public protocol {
private:
	static fss1bit fss;
	char** rom[2];
	char** wom;
	char** stash[2];
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
	void block_pir(const long addr_with_flag_23[2],
			const char* const * const mem_23[2], char* block_23[2],
			char* fss_out[2]);
	void rec_pir(const int idx_23[2], const char* const block_23[2],
			char* rec_23[2]);
	void gen_delta_array(const int idx_23[2], int numChunk, int chunkBytes,
			const char* const delta_23[2], char* delta_array_23[2]);
public:
	dpforam(const char* party, connection* cons[2],
			CryptoPP::AutoSeededRandomPool* rnd,
			CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs, int tau,
			int logN, int DBytes, bool isLast);
	~dpforam();
	void access(const long addr_23[2], const char* const newRec_23[2],
			bool isRead);
	void print_metadata();
	void test();
};

#endif /* DPFORAM_H_ */
