#include <assert.h>
#include <openssl/rand.h>

#include "util.h"

void cal_xor(const uchar* a, const uchar* b, uint bytes, uchar* c) {
	for (uint i = 0; i < bytes; i++) {
		c[i] = a[i] ^ b[i];
	}
}

void int_to_bytes(uint n, uchar* b) {
	b[0] = (n >> 24) & 0xFF;
	b[1] = (n >> 16) & 0xFF;
	b[2] = (n >> 8) & 0xFF;
	b[3] = n & 0xFF;
}

uint bytes_to_int(const uchar* b) {
	uint num = 0;
	for (uint i = 0; i < 4; i++) {
		num <<= 8;
		num |= b[i];
	}
	return num;
}

void long_to_bytes(ulong n, uchar* b) {
	b[0] = (n >> 56) & 0xFF;
	b[1] = (n >> 48) & 0xFF;
	b[2] = (n >> 40) & 0xFF;
	b[3] = (n >> 32) & 0xFF;
	b[4] = (n >> 24) & 0xFF;
	b[5] = (n >> 16) & 0xFF;
	b[6] = (n >> 8) & 0xFF;
	b[7] = n & 0xFF;
}

ulong bytes_to_long(const uchar* b) {
	ulong num = 0;
	for (uint i = 0; i < 8; i++) {
		num <<= 8;
		num |= b[i];
	}
	return num;
}

ulong rand_long(long range) {
	assert(range > 0);
	ulong bits;
	ulong val;
	uchar bytes[8];
	do {
		RAND_bytes(bytes, 8);
		bits = bytes_to_long(bytes);
		bits = (bits << 1) >> 1;
		val = bits % range;
	} while (bits - val + (range - 1) < 0L);
	return val;
}
