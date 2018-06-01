#include <string.h>

#include "prg256.h"

prg256::prg256() {
	unsigned char userKey[] = "abcdefghijklmnop";
	AES_set_encrypt_key(userKey, BLOCK_BYTES * 8, &key);
}

void prg256::compute(const char iv[BLOCK_BYTES], char out[BLOCK_BYTES * 2]) {
	unsigned char in[BLOCK_BYTES * 2] = { 0 };
	unsigned char ecount_buf[BLOCK_BYTES];
	unsigned int num = 0;
	unsigned char ivec[BLOCK_BYTES];
	memcpy(ivec, iv, BLOCK_BYTES);
	AES_ctr128_encrypt(in, (unsigned char*) out, BLOCK_BYTES * 2, &key, ivec,
			ecount_buf, &num);
}
