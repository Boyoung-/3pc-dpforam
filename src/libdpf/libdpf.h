#ifndef LIBDPF_LIBDPF_H_
#define LIBDPF_LIBDPF_H_

#include "aes.h"
#include "block.h"

void GEN(AES_KEY *key, int alpha, int n, unsigned char** k0,
		unsigned char **k1);
block EVAL(AES_KEY *key, unsigned char* k, int x);
block* EVALFULL(AES_KEY *key, const unsigned char* k);

#endif /* LIBDPF_LIBDPF_H_ */
