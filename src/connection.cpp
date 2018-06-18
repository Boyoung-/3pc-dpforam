#include "connection.h"
#include "util.h"

connection::~connection() {
}

void connection::write_int(int n) {
	unsigned char b[4];
	int_to_bytes(n, b);
	write((char*) b, 4);
}

int connection::read_int() {
	unsigned char b[4];
	read((char*) b, 4);
	return bytes_to_int(b);
}
