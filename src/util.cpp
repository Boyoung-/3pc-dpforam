#include "util.h"

char* get_xor(const char* a, const char* b, int bytes) {
	char* out = new char[bytes];
	for (int i = 0; i < bytes; i++) {
		out[i] = (char) (a[i] ^ b[i]);
	}
	return out;
}
