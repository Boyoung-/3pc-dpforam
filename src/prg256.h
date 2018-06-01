#ifndef PRG256_H_
#define PRG256_H_

#include <openssl/aes.h>

#define BLOCK_BYTES 16

class prg256 {
private:
	AES_KEY key;
public:
	prg256();
	void compute(const char iv[BLOCK_BYTES], char out[BLOCK_BYTES * 2]);
};

#endif /* PRG256_H_ */
