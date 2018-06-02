#include <iostream>
#include <openssl/rand.h>
#include <string.h>

#include "prg256.h"

prg256::prg256() {
	unsigned char userKey[] = "abcdefghijklmnop";
	AES_set_encrypt_key(userKey, BLOCK_BYTES * 8, &key);
}

void prg256::compute(const char seed[BLOCK_BYTES], char out[BLOCK_BYTES * 2]) {
	unsigned char in[BLOCK_BYTES * 2] = { 0 };
	unsigned char ecount_buf[BLOCK_BYTES];
	unsigned int num = 0;
	unsigned char ivec[BLOCK_BYTES];
	memcpy(ivec, seed, BLOCK_BYTES);
	AES_ctr128_encrypt(in, (unsigned char*) out, BLOCK_BYTES * 2, &key, ivec,
			ecount_buf, &num);
}

void prg256::compute(const char seed[BLOCK_BYTES], char out1[BLOCK_BYTES],
		char out2[BLOCK_BYTES]) {
	unsigned char in[BLOCK_BYTES] = { 0 };
	unsigned char ecount_buf[BLOCK_BYTES];
	unsigned int num = 0;
	unsigned char ivec[BLOCK_BYTES];
	memcpy(ivec, seed, BLOCK_BYTES);
	AES_ctr128_encrypt(in, (unsigned char*) out1, BLOCK_BYTES, &key, ivec,
			ecount_buf, &num);
	AES_ctr128_encrypt(in, (unsigned char*) out2, BLOCK_BYTES, &key, ivec,
			ecount_buf, &num);
}

void test_prg256() {
	prg256 prg1;
	prg256 prg2;
	for (int i = 0; i < 100; i++) {
		char seed[16];
		RAND_bytes((unsigned char*) seed, 16);
		char out1[32];
		char out2[32];
		prg1.compute(seed, out1);
		prg2.compute(seed, out2);
		if (memcmp(out1, out2, 32) == 0) {
			std::cout << "success: " << i << std::endl;
		} else {
			std::cerr << "!!! error: " << i << std::endl;
		}
	}
}
