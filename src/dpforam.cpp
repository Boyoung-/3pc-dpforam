#include <algorithm>
#include <iostream>

#include "dpforam.h"
#include "util.h"

fss1bit dpforam::fss;

void dpforam::init() {
	init_ctr();

	set_zero(rom[0]);
	set_zero(rom[1]);
	set_zero(wom);
	if (stash != NULL) {
		set_zero(stash[0]);
		set_zero(stash[1]);
	}

	if (!isFirst) {
		pos_map->init();
	}
}

void dpforam::init_ctr() {
	stash_ctr = 1;
}

void dpforam::set_zero(char** mem) {
	if (mem == NULL) {
		return;
	}

	for (long i = 0; i < N; i++) {
		memset(mem[i], 0, DBytes);
	}
}

void dpforam::init_mem(char** &mem) {
	mem = new char*[N];
	for (long i = 0; i < N; i++) {
		mem[i] = new char[DBytes];
	}
}

void dpforam::delete_mem(char** mem) {
	for (long i = 0; i < N; i++) {
		delete[] mem[i];
	}
	delete[] mem;
}

int dpforam::cal_last_tau(int DBytes) {
	if (DBytes < 2) {
		return 4;
	} else if (DBytes < 4) {
		return 3;
	} else if (DBytes < 8) {
		return 2;
	} else if (DBytes < 16) {
		return 1;
	} else {
		return 0;
	}
}

void dpforam::block_pir(const long addr_with_flag_23[2],
		const char* const * const mem_23[2], char* rec_23[2], char* t[2]) {
	long mask = N - 1;
	long addr_23[2];
	addr_23[0] = addr_with_flag_23[0] & mask;
	addr_23[1] = addr_with_flag_23[1] & mask;

	char* keys[2];
	int keyBytes = fss.gen(addr_23[0] ^ addr_23[1], logN, keys);
	cons[0]->write(keys[0], keyBytes);
	cons[1]->write(keys[1], keyBytes);
	cons[0]->read(keys[1], keyBytes);
	cons[1]->read(keys[0], keyBytes);

	memset(rec_23[0], 0, DBytes);
	for (int i = 0; i < 2; i++) {
		fss.eval_all_with_shift(keys[0], logN, addr_23[i], t[i]);
		for (long j = 0; j < N; j++) {
			if (t[i][j] == 1) {
				cal_xor(rec_23[0], mem_23[i][j], DBytes, rec_23[0]);
			}
		}
	}

	cons[0]->write(rec_23[0], DBytes);
	cons[1]->read(rec_23[1], DBytes);

	delete[] keys[0];
	delete[] keys[1];
}

dpforam::dpforam(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs, int tau, int logN,
		int DBytes, bool isLast) :
		protocol(party, cons, rnd, prgs) {
	this->isLast = isLast;
	this->tau = isLast ? cal_last_tau(DBytes) : tau;
	this->logN = isLast ? (logN - this->tau) : logN;
	ttp = 1 << this->tau;
	logNBytes = (this->logN + 7) / 8 + 1;
	nextLogN = isLast ? 0 : logN + tau;
	nextLogNBytes = isLast ? DBytes : (nextLogN + 7) / 8 + 1;
	this->DBytes = nextLogNBytes * ttp;
	N = 1L << this->logN;
	isFirst = this->logN - tau < tau;

	init_mem(rom[0]);
	init_mem(rom[1]);
	if (isFirst) {
		wom = NULL;
		pos_map = NULL;
	} else {
		init_mem(wom);
		init_mem(stash[0]);
		init_mem(stash[1]);
		pos_map = new dpforam(party, cons, rnd, prgs, tau, this->logN - tau, 0,
				false);
	}
	init_ctr();

	if (isLast) {
		init();
	}
}

dpforam::~dpforam() {
	if (!isFirst) {
		delete pos_map;
		delete_mem(stash[0]);
		delete_mem(stash[1]);
		delete_mem(wom);
	}
	delete_mem(rom[0]);
	delete_mem(rom[1]);
}

void dpforam::access(const long addr_23[2], const char* const newRec_23[2],
		bool isRead) {
	int mask = ttp - 1;
	long addrPre_23[2];
	int addrSuf_23[2];
	for (int i = 0; i < 2; i++) {
		addrPre_23[i] = addr_23[i] >> tau;
		addrSuf_23[i] = (int) addr_23[i] & mask;
	}
}

void dpforam::print_metadata() {
	std::cout << "===================" << std::endl;
	std::cout << "Party: " << party << std::endl;
	std::cout << "Last level: " << isLast << std::endl;
	std::cout << "First level: " << isFirst << std::endl;
	std::cout << "tau: " << tau << std::endl;
	std::cout << "2^tau: " << ttp << std::endl;
	std::cout << "logN: " << logN << std::endl;
	std::cout << "N: " << N << std::endl;
	std::cout << "logNBytes: " << logNBytes << std::endl;
	std::cout << "nextLogN: " << nextLogN << std::endl;
	std::cout << "nextLogNBytes: " << nextLogNBytes << std::endl;
	std::cout << "DBytes: " << DBytes << std::endl;
	std::cout << "Stash counter: " << stash_ctr << std::endl;
	std::cout << "ROM: " << (rom != NULL) << std::endl;
	std::cout << "WOM: " << (wom != NULL) << std::endl;
	std::cout << "stash: " << (stash != NULL) << std::endl;
	std::cout << "posMap: " << (pos_map != NULL) << std::endl;
	std::cout << "===================\n" << std::endl;

	if (!isFirst) {
		pos_map->print_metadata();
	}
}

void dpforam::test() {
	print_metadata();
}
