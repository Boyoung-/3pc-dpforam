#include <algorithm>
#include <iostream>
#include <stdint.h>

#include "fss.h"
#include "util.h"

void to_byte_vector(uint64_t input, uchar* output, uint size) {
	for (uint i = 0; i < size; i++) {
		output[i] = input & 1;
		input >>= 1;
	}
}

void to_byte_vector(uint64_t input, uchar* output) {
	to_byte_vector(input, output, 64);
}

void to_byte_vector(block input, uchar* output, uint size) {
	uint64_t *val = (uint64_t *) &input;
	to_byte_vector(val[0], output, size);
}

void to_byte_vector(block input, uchar* output) {
	uint64_t *val = (uint64_t *) &input;
	to_byte_vector(val[0], output);
	to_byte_vector(val[1], output + 64);
}

fss1bit::fss1bit() {
	long long userkey1 = 597349;
	long long userkey2 = 121379;
	block userkey = dpf_make_block(userkey1, userkey2);
	uchar seed[] = "abcdefghijklmnop";
	dpf_seed((block*) seed);
	AES_set_encrypt_key(userkey, &aes_key);
}

uint fss1bit::gen(ulong alpha, uint m, uchar* keys[2]) {
	return GEN(&aes_key, alpha, m, keys, keys + 1);
}

void fss1bit::eval_all(const uchar* key, uint m, uchar* out) {
	block* res = EVALFULL(&aes_key, key);
	if (m <= 6) {
		to_byte_vector(res[0], out, (1 << m));
	} else {
		uint maxlayer = std::max(m - 7, (uint) 0);
		ulong groups = 1L << maxlayer;
#pragma omp parallel for
		for (ulong i = 0; i < groups; i++) {
			to_byte_vector(res[i], out + i * 128);
		}
	}
	free(res);
}

// TODO: make this code more efficient?
void fss1bit::eval_all_with_shift(const uchar* key, uint m, ulong shift,
		uchar* out) {
	ulong range = 1L << m;
	uchar* tmp = new uchar[range];
	eval_all(key, m, tmp);
#pragma omp parallel for
	for (ulong i = 0; i < range; i++) {
		out[i] = tmp[i ^ shift];
	}
	delete[] tmp;
}

void test_fss() {
	fss1bit generator;
	fss1bit evaluators[2];

	for (uint m = 1; m <= 20; m++) {
		ulong range = 1L << m;

		for (uint i = 0; i < 100; i++) {
			bool pass = true;

			ulong alpha = rand_long(range);

			uchar* keys[2];
			generator.gen(alpha, m, keys);

			uchar* share0 = new uchar[range];
			uchar* share1 = new uchar[range];
			evaluators[0].eval_all(keys[0], m, share0);
			evaluators[1].eval_all(keys[1], m, share1);

			for (ulong x = 0; x < range; x++) {
				uchar output = share0[x] ^ share1[x];

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

