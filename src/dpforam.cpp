#include <algorithm>
#include <iostream>

#include "dpforam.h"

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

dpforam::dpforam(const char* party, connection* cons[2],
		CryptoPP::AutoSeededRandomPool* rnd,
		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs, int tau, int logN,
		int DBytes, bool isLast) :
		protocol(party, cons, rnd, prgs) {
	this->tau = tau;
	this->logN = logN;
	this->isLast = isLast;
	ttp = isLast ? std::max(16 / DBytes, 1) : (1 << tau);
	logNBytes = (logN + 7) / 8 + 1;
	nextLogN = isLast ? 0 : logN + tau;
	nextLogNBytes = isLast ? DBytes : (nextLogN + 7) / 8 + 1;
	this->DBytes = nextLogNBytes * ttp;
	N = 1L << logN;
	isFirst = logN - tau < tau;

	rom = new char**[2];
	init_mem(rom[0]);
	init_mem(rom[1]);
	if (isFirst) {
		wom = NULL;
		stash = NULL;
		pos_map = NULL;
	} else {
		init_mem(wom);
		stash = new char**[2];
		init_mem(stash[0]);
		init_mem(stash[1]);
		pos_map = new dpforam(party, cons, rnd, prgs, tau, logN - tau, 0,
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
		delete[] stash;
		delete_mem(wom);
	}
	delete_mem(rom[0]);
	delete_mem(rom[1]);
	delete[] rom;
}

void dpforam::print_metadata() {
	std::cout << "===================" << std::endl;
	std::cout << "Party: " << party << std::endl;
	std::cout << "tau: " << tau << std::endl;
	std::cout << "2^tau(or multiple): " << ttp << std::endl;
	std::cout << "Last level: " << isLast << std::endl;
	std::cout << "First level: " << isFirst << std::endl;
	std::cout << "N: " << N << std::endl;
	std::cout << "logN: " << logN << std::endl;
	std::cout << "logNBytes: " << logNBytes << std::endl;
	std::cout << "nextLogN: " << nextLogN << std::endl;
	std::cout << "nextLogNBytes: " << nextLogNBytes << std::endl;
	std::cout << "DBytes: " << DBytes << std::endl;
	std::cout << "Stash counter: " << stash_ctr << std::endl;
	std::cout << "ROM null: " << (rom == NULL) << std::endl;
	std::cout << "WOM null: " << (wom == NULL) << std::endl;
	std::cout << "stash null: " << (stash == NULL) << std::endl;
	std::cout << "posMap null: " << (pos_map == NULL) << std::endl;
	std::cout << "===================\n" << std::endl;

	if (!isFirst) {
		pos_map->print_metadata();
	}
}

void dpforam::test() {
	print_metadata();
}
