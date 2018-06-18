#ifndef FSS_H_
#define FSS_H_

#include "libdpf/libdpf.h"
#include "typedef.h"

class fss1bit {
private:
	AES_KEY aes_key;
public:
	fss1bit();
	uint gen(ulong alpha, uint m, uchar* keys[2]);
	void eval_all(const uchar* key, uint m, uchar* out);
	void eval_all_with_shift(const uchar* key, uint m, ulong shift, uchar* out);
};

void test_fss();

#endif /* FSS_H_ */
