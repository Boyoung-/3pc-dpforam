#ifndef PRG256_H_
#define PRG256_H_

#include <openssl/aes.h>

#define BLOCK_BYTES 16

class prg256 {
private:
	AES_KEY key;
public:
	prg256();
	void compute(const char seed[BLOCK_BYTES], char out[BLOCK_BYTES * 2]);
	void compute(const char seed[BLOCK_BYTES], char out1[BLOCK_BYTES],
			char out2[BLOCK_BYTES]);
};

void test_prg256();

#endif /* PRG256_H_ */
