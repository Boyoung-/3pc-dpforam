#include <algorithm>
#include <iostream>
#include <openssl/rand.h>
#include <string.h>

#include "fss.h"
#include "util.h"

fsskey::fsskey() {
	t = 0;
	mPlus1 = 0;
}

fsskey::~fsskey() {
	if (sigma != NULL) {
		for (int i = 0; i < mPlus1; i++) {
			delete[] sigma[i];
			delete[] tau[i];
		}
		delete[] sigma;
		delete[] tau;
	}
}

void fsskey::set_key(const char s[SEED_BYTES], char t,
		const char sigma[][SEED_BYTES], const char tau[][2], const char* gamma,
		int mPlus1) {
	memcpy(this->s, s, SEED_BYTES);
	this->t = t;
	this->sigma = new char*[mPlus1];
	this->tau = new char*[mPlus1];
	for (int i = 0; i < mPlus1; i++) {
		this->sigma[i] = new char[SEED_BYTES];
		this->tau[i] = new char[2];
		memcpy(this->sigma[i], sigma[i], SEED_BYTES);
		memcpy(this->tau[i], tau[i], 2);
	}
	if (gamma != NULL) {
		memcpy(this->gamma, gamma, SEED_BYTES);
	}
	this->mPlus1 = mPlus1;
}

void fsskey::set_gamma(const char gamma[SEED_BYTES]) {
	memcpy(this->gamma, gamma, SEED_BYTES);
}

void set_bit(char array[SEED_BYTES], int pos) {
	int byte_pos = SEED_BYTES - 1 - pos / 8;
	int bit_pos = 7 - pos % 8;
	array[byte_pos] |= (1 << bit_pos);
}

void fss1bit::gen(long alpha, int m, fsskey keys[2]) {
	long mask = (1 << m) - 1;
	alpha &= mask;
	int old_m = m;
	m = std::max(1, m - ET_BITS);

	int mPlus1 = m + 1;
	char s_a_p[mPlus1][SEED_BYTES];
	char s_b_p[mPlus1][SEED_BYTES];
	char t_a[mPlus1];
	char t_b[mPlus1];
	char s_a[2][mPlus1][SEED_BYTES];
	char s_b[2][mPlus1][SEED_BYTES];
	char sigma[mPlus1][SEED_BYTES];
	char tau[mPlus1][2];

	RAND_bytes((unsigned char*) s_a_p[0], SEED_BYTES);
	RAND_bytes((unsigned char*) s_b_p[0], SEED_BYTES);
	RAND_bytes((unsigned char*) t_a, 1);
	t_a[0] &= 1;
	t_b[0] = 1 - t_a[0];

	for (int j = 1; j <= m; j++) {
		prg.compute(s_a_p[j - 1], s_a[0][j], s_a[1][j]);
		prg.compute(s_b_p[j - 1], s_b[0][j], s_b[1][j]);
		int alpha_j = (int) (alpha & 1);
		alpha = alpha >> 1;
		cal_xor(s_a[1 - alpha_j][j], s_b[1 - alpha_j][j], SEED_BYTES, sigma[j]);

		tau[j][0] = (s_a[0][j][SEED_BYTES - 1] ^ s_b[0][j][SEED_BYTES - 1]
				^ alpha_j ^ 1) & 1;
		tau[j][1] = (s_a[1][j][SEED_BYTES - 1] ^ s_b[1][j][SEED_BYTES - 1]
				^ alpha_j) & 1;

		memcpy(s_a_p[j], s_a[alpha_j][j], SEED_BYTES);
		t_a[j] = s_a[alpha_j][j][SEED_BYTES - 1] & 1;
		if (t_a[j - 1] == 1) {
			cal_xor(s_a_p[j], sigma[j], SEED_BYTES, s_a_p[j]);
			t_a[j] = t_a[j] ^ tau[j][alpha_j];
		}
		memcpy(s_b_p[j], s_b[alpha_j][j], SEED_BYTES);
		t_b[j] = s_b[alpha_j][j][SEED_BYTES - 1] & 1;
		if (t_b[j - 1] == 1) {
			cal_xor(s_b_p[j], sigma[j], SEED_BYTES, s_b_p[j]);
			t_b[j] = t_b[j] ^ tau[j][alpha_j];
		}
	}

	keys[0].set_key(s_a_p[0], t_a[0], sigma, tau, NULL, mPlus1);
	keys[1].set_key(s_b_p[0], t_b[0], sigma, tau, NULL, mPlus1);

	if (old_m == 1) {
		return;
	}

	char gamma[SEED_BYTES] = { 0 };
	set_bit(gamma, (int) alpha);
	cal_xor(gamma, s_a_p[m], SEED_BYTES, gamma);
	cal_xor(gamma, s_b_p[m], SEED_BYTES, gamma);
	keys[0].set_gamma(gamma);
	keys[1].set_gamma(gamma);
}

void test_fss() {
	for (int m = 1; m <= 20; m++) {
		fss1bit dpf;
		fsskey keys[2];
		dpf.gen(0, m, keys);
		std::cout << "success " << m << std::endl;
	}
}
