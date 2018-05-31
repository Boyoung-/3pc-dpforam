#include "util.h"

void cal_xor(const char* a, const char* b, int bytes, char* c) {
	for (int i = 0; i < bytes; i++) {
		c[i] = (char) (a[i] ^ b[i]);
	}
}

void int_to_bytes(int n, char* b) {
	b[0] = (n >> 24) & 0xFF;
	b[1] = (n >> 16) & 0xFF;
	b[2] = (n >> 8) & 0xFF;
	b[3] = n & 0xFF;
}

int bytes_to_int(const char* b) {
	int num = 0;
	for (int i = 0; i < 4; i++) {
		num <<= 8;
		num |= b[i];
	}
	return num;
}
