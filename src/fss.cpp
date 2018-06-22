#include <algorithm>
#include <assert.h>
#include <iostream>

#include "fss.h"
#include "util.h"

const ulong masks[64] = { 0x0000000000000001ul, 0x0000000000000002ul,
		0x0000000000000004ul, 0x0000000000000008ul, 0x0000000000000010ul,
		0x0000000000000020ul, 0x0000000000000040ul, 0x0000000000000080ul,
		0x0000000000000100ul, 0x0000000000000200ul, 0x0000000000000400ul,
		0x0000000000000800ul, 0x0000000000001000ul, 0x0000000000002000ul,
		0x0000000000004000ul, 0x0000000000008000ul, 0x0000000000010000ul,
		0x0000000000020000ul, 0x0000000000040000ul, 0x0000000000080000ul,
		0x0000000000100000ul, 0x0000000000200000ul, 0x0000000000400000ul,
		0x0000000000800000ul, 0x0000000001000000ul, 0x0000000002000000ul,
		0x0000000004000000ul, 0x0000000008000000ul, 0x0000000010000000ul,
		0x0000000020000000ul, 0x0000000040000000ul, 0x0000000080000000ul,
		0x0000000100000000ul, 0x0000000200000000ul, 0x0000000400000000ul,
		0x0000000800000000ul, 0x0000001000000000ul, 0x0000002000000000ul,
		0x0000004000000000ul, 0x0000008000000000ul, 0x0000010000000000ul,
		0x0000020000000000ul, 0x0000040000000000ul, 0x0000080000000000ul,
		0x0000100000000000ul, 0x0000200000000000ul, 0x0000400000000000ul,
		0x0000800000000000ul, 0x0001000000000000ul, 0x0002000000000000ul,
		0x0004000000000000ul, 0x0008000000000000ul, 0x0010000000000000ul,
		0x0020000000000000ul, 0x0040000000000000ul, 0x0080000000000000ul,
		0x0100000000000000ul, 0x0200000000000000ul, 0x0400000000000000ul,
		0x0800000000000000ul, 0x1000000000000000ul, 0x2000000000000000ul,
		0x4000000000000000ul, 0x8000000000000000ul };

void to_byte_vector(ulong input, uchar* output, uint size) {
#pragma omp simd aligned(output,masks:16)
	for (uint i = 0; i < size; i++) {
		output[i] = (input & masks[i]) != 0ul;
	}
}

void to_byte_vector(block input, uchar* output) {
	ulong *val = (ulong *) &input;
	to_byte_vector(val[0], output, 64);
	to_byte_vector(val[1], output + 64, 64);
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
		to_byte_vector(((ulong*) res)[0], out, (1 << m));
	} else {
		uint maxlayer = std::max(m - 7, 0u);
		ulong groups = 1ul << maxlayer;
//#pragma omp parallel for
		for (ulong i = 0; i < groups; i++) {
			to_byte_vector(res[i], out + (i << 7));
		}
	}
	free(res);
}

// TODO: make this code more efficient?
void fss1bit::eval_all_with_perm(const uchar* key, uint m, ulong perm,
		uchar* out) {
	ulong range = 1ul << m;
	uchar* tmp = new uchar[range];
	eval_all(key, m, tmp);
//#pragma omp simd
	for (ulong i = 0; i < range; i++) {
		out[i] = tmp[i ^ perm];
	}
	delete[] tmp;
}

void test_fss() {
	fss1bit generator;
	fss1bit evaluators[2];

	for (uint m = 1; m <= 20; m++) {
		ulong range = 1ul << m;

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
					if (output == 0) {
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

			delete[] keys[0];
			delete[] keys[1];
			delete[] share0;
			delete[] share1;
		}
		std::cout << std::endl;
	}
}

