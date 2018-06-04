#ifndef UTIL_H_
#define UTIL_H_

void cal_xor(const char* a, const char* b, int bytes, char* c);
void int_to_bytes(int n, char* b);
int bytes_to_int(const char* b);
void long_to_bytes(long n, char* b);
long bytes_to_long(const char* b);
long rand_long(long range);

#endif /* UTIL_H_ */
