#include <algorithm>
#include <iostream>
#include <stdint.h>

#include "fss.h"
#include "util.h"

void to_byte_vector(uint64_t input, char* output, int size) {
	for (int i = 0; i < size; i++) {
		output[i] = input & 1;
		input >>= 1;
	}
}

void to_byte_vector(uint64_t input, char* output) {
	to_byte_vector(input, output, 64);
}

void to_byte_vector(block input, char* output, int size) {
	uint64_t *val = (uint64_t *) &input;
	to_byte_vector(val[0], output, size);
}

void to_byte_vector(block input, char* output) {
	uint64_t *val = (uint64_t *) &input;
	to_byte_vector(val[0], output);
	to_byte_vector(val[1], output + 64);
}

fss1bit::fss1bit() {
	long long userkey1 = 597349;
	long long userkey2 = 121379;
	block userkey = dpf_make_block(userkey1, userkey2);
	unsigned char seed[] = "abcdefghijklmnop";
	dpf_seed((block*) seed);
	AES_set_encrypt_key(userkey, &aes_key);
}

int fss1bit::gen(long alpha, int m, char* keys[2]) {
	return GEN(&aes_key, alpha, m, (unsigned char**) keys,
			(unsigned char**) keys + 1);
}

void fss1bit::eval_all(const char* key, int m, char* out) {
	block* res = EVALFULL(&aes_key, (const unsigned char*) key);
	if (m <= 6) {
		to_byte_vector(res[0], out, (1 << m));
	} else {
		int maxlayer = std::max(m - 7, 0);
		long groups = 1L << maxlayer;
#pragma omp parallel for
		for (long i = 0; i < groups; i++) {
			to_byte_vector(res[i], out + i * 128);
		}
	}
	free(res);
}

// TODO: make this code more efficient?
void fss1bit::eval_all_with_shift(const char* key, int m, long shift,
		char* out) {
	long range = 1L << m;
	char* tmp = new char[range];
	eval_all(key, m, tmp);
#pragma omp parallel for
	for (long i = 0; i < range; i++) {
		out[i] = tmp[i ^ shift];
	}
	delete[] tmp;
}

void test_fss() {
	fss1bit generator;
	fss1bit evaluators[2];

	for (int m = 1; m <= 20; m++) {
		long range = 1L << m;

		for (int i = 0; i < 100; i++) {
			bool pass = true;

			long alpha = rand_long(range);

			char* keys[2];
			generator.gen(alpha, m, keys);

			char* share0 = new char[range];
			char* share1 = new char[range];
			evaluators[0].eval_all(keys[0], m, share0);
			evaluators[1].eval_all(keys[1], m, share1);

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

			free(keys[0]);
			free(keys[1]);
			delete[] share0;
			delete[] share1;
		}
		std::cout << std::endl;
	}
}

