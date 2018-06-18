#include "connection.h"
#include "util.h"

connection::~connection() {
}

void connection::write_int(int n) {
	uchar b[4];
	int_to_bytes(n, b);
	write(b, 4);
}

int connection::read_int() {
	uchar b[4];
	read(b, 4);
	return bytes_to_int(b);
}
