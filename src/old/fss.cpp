#include <algorithm>
#include <iostream>
#include <math.h>
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

int test_bit(const char array[SEED_BYTES], int pos) {
	int byte_pos = SEED_BYTES - 1 - pos / 8;
	int bit_pos = 7 - pos % 8;
	return (array[byte_pos] >> bit_pos) & 1;
}

long reverse(long n, int bits) {
	long res = 0;
	for (int i = 0; i < bits; i++) {
		res = (res << 1) | (n & 1);
		n = n >> 1;
	}
	return res;
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

void fss1bit::eval_all(const fsskey &key, int m, char* out, long out_size) {
	int old_m = m;
	m = std::max(1, m - ET_BITS);

	long size = 1;
	char** prev_s = new char*[size];
	char** next_s = NULL;
	char* prev_t = new char[size];
	char* next_t = NULL;
	prev_s[0] = new char[SEED_BYTES];
	memcpy(prev_s[0], key.s, SEED_BYTES);
	prev_t[0] = key.t;

	for (int j = 1; j <= m; j++) {
		long width = size * 2;
		next_s = new char*[width];
		for (long i = 0; i < width; i++) {
			next_s[i] = new char[SEED_BYTES];
		}
		next_t = new char[width];

		for (long i = 0; i < size; i++) {
			long left = 2 * i;
			long right = left + 1;
			prg.compute(prev_s[i], next_s[left], next_s[right]);

			next_t[left] = next_s[left][SEED_BYTES - 1] & 1;
			next_t[right] = next_s[right][SEED_BYTES - 1] & 1;

			if (prev_t[i] == 1) {
				cal_xor(next_s[left], key.sigma[j], SEED_BYTES, next_s[left]);
				cal_xor(next_s[right], key.sigma[j], SEED_BYTES, next_s[right]);

				next_t[left] ^= key.tau[j][0];
				next_t[right] ^= key.tau[j][1];
			}
		}

		for (long i = 0; i < size; i++) {
			delete[] prev_s[i];
		}
		delete[] prev_s;
		delete[] prev_t;
		prev_s = next_s;
		prev_t = next_t;
		size = width;
	}

	if (old_m == 1) {
		memcpy(out, next_t, out_size);
		return;
	}

	int diff_m = old_m - m;
	for (long i = 0; i < size; i++) {
		if (next_t[i] == 1) {
			cal_xor(next_s[i], key.gamma, SEED_BYTES, next_s[i]);
		}

		for (int j = 0; j < (int) pow(2, diff_m); j++) {
			long index = (((long) j) << m) | reverse(i, m);
			out[index] = test_bit(next_s[i], j);
		}
	}

	for (long i = 0; i < size; i++) {
		delete[] next_s[i];
	}
	delete[] next_s;
	delete[] next_t;
}

void test_fss() {
	for (int m = 1; m <= 20; m++) {
		long range = (long) pow(2, m);

		for (int i = 0; i < 100; i++) {
			bool pass = true;

			long alpha = rand_long(range);

			fss1bit dpf;
			fsskey keys[2];
			dpf.gen(alpha, m, keys);

			char share0[range];
			char share1[range];
			dpf.eval_all(keys[0], m, share0, range);
			dpf.eval_all(keys[1], m, share1, range);

			for (long x = 0; x < range; x++) {
				int output = share0[x] ^ share1[x];

				if (x == alpha) {
					if (output != 1) {
						std::cout << "Failed: alpha=" << alpha << ", x=" << x
								<< ", outValue=" << output << std::endl;
						pass = false;
					}
				} else {
					if (output != 0) {
						std::cout << "Failed: alpha=" << alpha << ", x=" << x
								<< ", outValue=" << output << std::endl;
						pass = false;
					}
				}
			}

			if (pass)
				std::cout << "m=" << m << ", i=" << i << ": passed"
						<< std::endl;
			else
				std::cout << "m=" << m << ", i=" << i << ": failed"
						<< std::endl;
		}
		std::cout << std::endl;
	}
}
