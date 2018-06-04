#ifndef FSS_H_
#define FSS_H_

#define SEED_BYTES 16
#define ET_BITS 7

#include "prg256.h"

class fsskey {
public:
	char s[SEED_BYTES];
	char t;
	char** sigma = NULL;
	char** tau = NULL;
	char gamma[SEED_BYTES];
	int mPlus1;

	fsskey();
	~fsskey();

	void set_key(const char s[SEED_BYTES], char t,
			const char sigma[][SEED_BYTES], const char tau[][2],
			const char* gamma, int mPlus1);
	void set_gamma(const char gamma[SEED_BYTES]);
};

class fss1bit {
private:
	prg256 prg;
public:
	void gen(long alpha, int m, fsskey keys[2]);
	void eval_all(const fsskey &key, int m, char* out, long out_size);
};

void test_fss();

#endif /* FSS_H_ */
