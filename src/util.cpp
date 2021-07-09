#include "util.h"

#include <assert.h>
#include <emmintrin.h>
#include <openssl/rand.h>
#include <string.h>
#include <sys/time.h>

#include <algorithm>

void cal_xor(const uchar *a, const uchar *b, uint bytes, uchar *c) {
    for (uint i = 0; i < bytes; i++) {
        c[i] = a[i] ^ b[i];
    }
}

// void cal_xor_128(const uchar *a, const uchar *b, uint quo, uint rem, uchar *c)
// {
//     __m128i *aa = (__m128i *)a;
//     __m128i *bb = (__m128i *)b;
//     __m128i *cc = (__m128i *)c;
//     uint i;
//     for (i = 0; i < quo; i++)
//     {
//         cc[i] = _mm_xor_si128(aa[i], bb[i]);
//     }
//     if (rem)
//     {
//         a = (uchar *)&(aa[i]);
//         b = (uchar *)&(bb[i]);
//         c = (uchar *)&(cc[i]);
// #pragma omp simd
//         for (i = 0; i < rem; i++)
//         {
//             c[i] = a[i] ^ b[i];
//         }
//     }
// }

void int_to_bytes(uint n, uchar *b) {
    b[0] = (n >> 24) & 0xFF;
    b[1] = (n >> 16) & 0xFF;
    b[2] = (n >> 8) & 0xFF;
    b[3] = n & 0xFF;
}

uint bytes_to_int(const uchar *b) {
    uint num = 0;
    for (uint i = 0; i < 4; i++) {
        num <<= 8;
        num |= b[i];
    }
    return num;
}

void long_to_bytes(unsigned long n, uchar *b) {
    b[0] = (n >> 56) & 0xFF;
    b[1] = (n >> 48) & 0xFF;
    b[2] = (n >> 40) & 0xFF;
    b[3] = (n >> 32) & 0xFF;
    b[4] = (n >> 24) & 0xFF;
    b[5] = (n >> 16) & 0xFF;
    b[6] = (n >> 8) & 0xFF;
    b[7] = n & 0xFF;
}

void long_to_bytes(unsigned long n, uchar *b, uint len) {
    for (uint i = 0; i < std::min(len, 8u); i++) {
        b[len - 1 - i] = n & 0xFF;
        n >>= 8;
    }
    if (len > 8) {
        memset(b, 0, len - 8);
    }
}

unsigned long bytes_to_long(const uchar *b) {
    unsigned long num = 0;
    for (uint i = 0; i < 8; i++) {
        num <<= 8;
        num |= b[i];
    }
    return num;
}

unsigned long bytes_to_long(const uchar *b, uint len) {
    unsigned long num = 0;
    uint min = std::min(len, 8u);
    for (uint i = 0; i < min; i++) {
        num <<= 8;
        num |= b[len - min + i];
    }
    return num;
}

unsigned long rand_long(long range) {
    assert(range > 0);
    unsigned long bits;
    long val;
    uchar bytes[8];
    do {
        RAND_bytes(bytes, 8);
        bits = bytes_to_long(bytes);
        bits = (bits << 1) >> 1;
        val = bits % range;
    } while ((long)bits - val + (range - 1L) < 0L);
    return val;
}

unsigned long current_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long)tv.tv_sec * (unsigned long)1000000 + (unsigned long)tv.tv_usec;
}
