#ifndef UTIL_H_
#define UTIL_H_

#include "typedef.h"
#include "libdpf/block.h"

const __m128i masks_128[2] = {_mm_set_epi32(0, 0, 0, 0), _mm_set_epi32(
                                                             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)};
const uchar masks_8[2] = {0x00, 0xFF};

void cal_xor(const uchar *a, const uchar *b, uint bytes, uchar *c);
// void cal_xor_128(const uchar *a, const uchar *b, uint quo, uint rem, uchar *c);
inline void set_xor_128(const uchar *__restrict__ a, uint quo, uint rem,
                        uchar *__restrict__ c)
{
    __m128i *aa = (__m128i *)a;
    __m128i *cc = (__m128i *)c;
    uint i;
    for (i = 0; i < quo; i++)
    {
        cc[i] = _mm_xor_si128(aa[i], cc[i]);
    }
    if (rem)
    {
        a = (uchar *)&(aa[i]);
        c = (uchar *)&(cc[i]);
#pragma omp simd
        for (i = 0; i < rem; i++)
        {
            c[i] ^= a[i];
        }
    }
}

inline void select_xor_128(const uchar *__restrict__ a, bool bit, uint quo, uint rem,
                           uchar *__restrict__ c)
{
    __m128i *aa = (__m128i *)a;
    __m128i *cc = (__m128i *)c;
    uint i;
    for (i = 0; i < quo; i++)
    {
        cc[i] = _mm_xor_si128(_mm_and_si128(aa[i], masks_128[bit]), cc[i]);
    }
    if (rem)
    {
        a = (uchar *)&(aa[i]);
        c = (uchar *)&(cc[i]);
#pragma omp simd
        for (i = 0; i < rem; i++)
        {
            c[i] ^= (a[i] & masks_8[bit]);
        }
    }
}

void int_to_bytes(uint n, uchar *b);
uint bytes_to_int(const uchar *b);
void long_to_bytes(unsigned long n, uchar *b);
void long_to_bytes(unsigned long n, uchar *b, uint len);
unsigned long bytes_to_long(const uchar *b);
unsigned long bytes_to_long(const uchar *b, uint len);
unsigned long rand_long(long range);
unsigned long current_timestamp();

#endif /* UTIL_H_ */
