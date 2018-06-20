#include "connection.h"

connection::~connection() {
}

void connection::write_int(int n) {
	write((uchar*) &n, 4);
}

int connection::read_int() {
	int n;
	read((uchar*) &n, 4);
	return n;
}
