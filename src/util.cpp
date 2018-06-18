#include <openssl/rand.h>

#include "util.h"

void cal_xor(const char* a, const char* b, int bytes, char* c) {
	for (int i = 0; i < bytes; i++) {
		c[i] = (char) (a[i] ^ b[i]);
	}
}

void int_to_bytes(unsigned n, unsigned char* b) {
	b[0] = (n >> 24) & 0xFF;
	b[1] = (n >> 16) & 0xFF;
	b[2] = (n >> 8) & 0xFF;
	b[3] = n & 0xFF;
}

unsigned bytes_to_int(const unsigned char* b) {
	unsigned num = 0;
	for (int i = 0; i < 4; i++) {
		num <<= 8;
		num |= b[i];
	}
	return num;
}

void long_to_bytes(unsigned long n, unsigned char* b) {
	b[0] = (n >> 56) & 0xFF;
	b[1] = (n >> 48) & 0xFF;
	b[2] = (n >> 40) & 0xFF;
	b[3] = (n >> 32) & 0xFF;
	b[4] = (n >> 24) & 0xFF;
	b[5] = (n >> 16) & 0xFF;
	b[6] = (n >> 8) & 0xFF;
	b[7] = n & 0xFF;
}

unsigned long bytes_to_long(const unsigned char* b) {
	unsigned long num = 0;
	for (int i = 0; i < 8; i++) {
		num <<= 8;
		num |= b[i];
	}
	return num;
}

long rand_long(long range) {
	unsigned long bits;
	unsigned long val;
	unsigned char bytes[8];
	do {
		RAND_bytes((unsigned char*) bytes, 8);
		bits = bytes_to_long(bytes);
		bits = (bits << 1) >> 1;
		val = bits % range;
	} while (bits - val + (range - 1) < 0L);
	return (long) val;
}
