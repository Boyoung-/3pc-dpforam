#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdint.h>

#include "fss.h"
#include "util.h"

void to_bit_vector(uint64_t input, char* output) {
	for (int i = 0; i < 64; i++) {
		output[i] = input & 1;
		input >>= 1;
	}
}

void to_bit_vector(block input, char* output) {
	uint64_t *val = (uint64_t *) &input;
	to_bit_vector(val[0], output);
	to_bit_vector(val[1], output + 64);
}

fss1bit::fss1bit() {
	long long userkey1 = 597349;
	long long userkey2 = 121379;
	block userkey = dpf_make_block(userkey1, userkey2);
	unsigned char seed[] = "abcdefghijklmnop";
	dpf_seed((block*) seed);
	AES_set_encrypt_key(userkey, &aes_key);
}

void fss1bit::gen(long alpha, int m, char* keys[2]) {
	GEN(&aes_key, alpha, m, (unsigned char**) keys, (unsigned char**) keys + 1);
}

// TODO: make use of out_size
void fss1bit::eval_all(const char* key, int m, char* out, long out_size) {
	block* res = EVALFULL(&aes_key, (const unsigned char*) key);
	int maxlayer = std::max(m - 7, 0);
	long groups = (long) pow(2, maxlayer);
	for (long i = 0; i < groups; i++) {
		to_bit_vector(res[i], out + i * 128);
	}
	free(res);
}

void test_fss() {
	fss1bit generator;
	fss1bit evaluators[2];

	for (int m = 1; m <= 20; m++) {
		long range = (long) pow(2, m);

		for (int i = 0; i < 100; i++) {
			bool pass = true;

			long alpha = rand_long(range);

			char* keys[2];
			generator.gen(alpha, m, keys);

			char share0[std::max(range, 128L)];
			char share1[std::max(range, 128L)];
			evaluators[0].eval_all(keys[0], m, share0, std::max(range, 128L));
			evaluators[1].eval_all(keys[1], m, share1, std::max(range, 128L));

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
		}
		std::cout << std::endl;
	}
}

