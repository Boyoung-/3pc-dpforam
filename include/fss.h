#ifndef FSS_H_
#define FSS_H_

#include "libdpf/libdpf.h"
#include "typedef.h"

class fss1bit
{
private:
    AES_KEY aes_key;

public:
    fss1bit();
    uint gen(unsigned long alpha, uint m, uchar *keys[2]);
    void eval_all(const uchar *key, uint m, uchar *out);
    void eval_all_with_perm(const uchar *key, uint m, unsigned long perm, uchar *out);
};

#endif /* FSS_H_ */
