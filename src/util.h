#ifndef UTIL_H_
#define UTIL_H_

void cal_xor(const char* a, const char* b, int bytes, char* c);
void int_to_bytes(unsigned n, unsigned char* b);
unsigned bytes_to_int(const unsigned char* b);
void long_to_bytes(unsigned long n, unsigned char* b);
unsigned long bytes_to_long(const unsigned char* b);
long rand_long(long range);

#endif /* UTIL_H_ */
