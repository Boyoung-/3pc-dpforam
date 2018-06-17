#ifndef LIBDPF_LIBDPF_H_
#define LIBDPF_LIBDPF_H_

#include "aes.h"
#include "block.h"

int GEN(AES_KEY *key, long alpha, int n, unsigned char** k0,
		unsigned char **k1);
block EVAL(AES_KEY *key, unsigned char* k, long x);
block* EVALFULL(AES_KEY *key, const unsigned char* k);

void test_libdpf();

#endif /* LIBDPF_LIBDPF_H_ */
