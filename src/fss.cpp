#include "fss.h"

#include <string.h>
#include <x86intrin.h>

#include <algorithm>

#include "bit_perm.h"

const unsigned long masks[64] = {0x0000000000000001ul, 0x0000000000000002ul,
                                 0x0000000000000004ul, 0x0000000000000008ul, 0x0000000000000010ul,
                                 0x0000000000000020ul, 0x0000000000000040ul, 0x0000000000000080ul,
                                 0x0000000000000100ul, 0x0000000000000200ul, 0x0000000000000400ul,
                                 0x0000000000000800ul, 0x0000000000001000ul, 0x0000000000002000ul,
                                 0x0000000000004000ul, 0x0000000000008000ul, 0x0000000000010000ul,
                                 0x0000000000020000ul, 0x0000000000040000ul, 0x0000000000080000ul,
                                 0x0000000000100000ul, 0x0000000000200000ul, 0x0000000000400000ul,
                                 0x0000000000800000ul, 0x0000000001000000ul, 0x0000000002000000ul,
                                 0x0000000004000000ul, 0x0000000008000000ul, 0x0000000010000000ul,
                                 0x0000000020000000ul, 0x0000000040000000ul, 0x0000000080000000ul,
                                 0x0000000100000000ul, 0x0000000200000000ul, 0x0000000400000000ul,
                                 0x0000000800000000ul, 0x0000001000000000ul, 0x0000002000000000ul,
                                 0x0000004000000000ul, 0x0000008000000000ul, 0x0000010000000000ul,
                                 0x0000020000000000ul, 0x0000040000000000ul, 0x0000080000000000ul,
                                 0x0000100000000000ul, 0x0000200000000000ul, 0x0000400000000000ul,
                                 0x0000800000000000ul, 0x0001000000000000ul, 0x0002000000000000ul,
                                 0x0004000000000000ul, 0x0008000000000000ul, 0x0010000000000000ul,
                                 0x0020000000000000ul, 0x0040000000000000ul, 0x0080000000000000ul,
                                 0x0100000000000000ul, 0x0200000000000000ul, 0x0400000000000000ul,
                                 0x0800000000000000ul, 0x1000000000000000ul, 0x2000000000000000ul,
                                 0x4000000000000000ul, 0x8000000000000000ul};

void to_byte_vector(unsigned long input, uchar *output, uint size) {
#pragma omp simd aligned(output, masks : 16)
    for (uint i = 0; i < size; i++) {
        output[i] = (input & masks[i]) != 0ul;
    }
}

void to_byte_vector(block input, uchar *output) {
    unsigned long *val = (unsigned long *)&input;
    to_byte_vector(val[0], output, 64);
    to_byte_vector(val[1], output + 64, 64);
}

// TODO: find supported cpu to test BMI2
void to_byte_vector_with_perm(unsigned long input, uchar *output, uint size,
                              uint perm) {
//#if defined(__BMI2__)
//	input = general_reverse_bits(input, perm ^ 63);
//	uchar* addr = (uchar*) &input;
//	unsigned long long * data64 = (unsigned long long *) output;
//	for (uint i = 0; i < size/8; ++i) {
//		unsigned long long tmp = 0;
//		memcpy(&tmp, addr+i, 1);
//		data64[i] = _pdep_u64(tmp, (unsigned long long) 0x0101010101010101ULL);
//	}
//#else
#pragma omp simd aligned(output, masks : 16)
    for (uint i = 0; i < size; i++) {
        output[i] = (input & masks[i ^ perm]) != 0ul;
    }
    //#endif
}

fss1bit::fss1bit() {
    long long userkey1 = 597349;
    long long userkey2 = 121379;
    block userkey = dpf_make_block(userkey1, userkey2);
    uchar seed[] = "abcdefghijklmnop";
    dpf_seed((block *)seed);
    AES_set_encrypt_key(userkey, &aes_key);
}

uint fss1bit::gen(unsigned long alpha, uint m, uchar *keys[2]) {
    return GEN(&aes_key, alpha, m, keys, keys + 1);
}

void fss1bit::eval_all(const uchar *key, uint m, uchar *out) {
    block *res = EVALFULL(&aes_key, key);
    if (m <= 6) {
        to_byte_vector(((unsigned long *)res)[0], out, (1 << m));
    } else {
        uint maxlayer = std::max((int)m - 7, 0);
        unsigned long groups = 1ul << maxlayer;
        for (unsigned long i = 0; i < groups; i++) {
            to_byte_vector(res[i], out + (i << 7));
        }
    }
    free(res);
}

void fss1bit::eval_all_with_perm(const uchar *key, uint m, unsigned long perm,
                                 uchar *out) {
    block *res = EVALFULL(&aes_key, key);
    unsigned long *ptr = (unsigned long *)res;
    uint index_perm = perm & 63;
    if (m <= 6) {
        to_byte_vector_with_perm(ptr[0], out, (1 << m), index_perm);
    } else {
        unsigned long group_perm = perm >> 6;
        uint maxlayer = std::max((int)m - 6, 0);
        unsigned long groups = 1ul << maxlayer;
        //#pragma omp parallel for
        for (unsigned long i = 0; i < groups; i++) {
            to_byte_vector_with_perm(ptr[i ^ group_perm], out + (i << 6), 64,
                                     index_perm);
        }
    }
    free(res);
}
