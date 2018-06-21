#ifndef UTIL_H_
#define UTIL_H_

#include "typedef.h"

void cal_xor(const uchar* a, const uchar* b, uint bytes, uchar* c);
void cal_xor_128(const uchar* a, const uchar* b, uint bytes, uchar* c);
void int_to_bytes(uint n, uchar* b);
uint bytes_to_int(const uchar* b);
void long_to_bytes(ulong n, uchar* b);
void long_to_bytes(ulong n, uchar* b, uint len);
ulong bytes_to_long(const uchar* b);
ulong bytes_to_long(const uchar* b, uint len);
ulong rand_long(long range);

#endif /* UTIL_H_ */
