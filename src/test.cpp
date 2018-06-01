#include <iostream>
#include <openssl/rand.h>
#include <string.h>

#include "prg256.h"

using namespace std;

int main() {
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
			cout << "success: " << i << endl;
		} else {
			cerr << "!!! error: " << i << endl;
		}
	}

	return 0;
}
