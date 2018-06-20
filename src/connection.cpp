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

void connection::write_long(long n) {
	write((uchar*) &n, 8);
}

long connection::read_long() {
	long n;
	read((uchar*) &n, 8);
	return n;
}
