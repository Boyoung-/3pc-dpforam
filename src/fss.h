#ifndef FSS_H_
#define FSS_H_

#include "libdpf/libdpf.h"

class fss1bit {
private:
	AES_KEY aes_key;
public:
	fss1bit();
	void gen(long alpha, int m, char* keys[2]);
	void eval_all(const char* key, int m, char* out, long out_size);
};

void test_fss();

#endif /* FSS_H_ */
