#include "connection.h"

connection::~connection() {
}

void connection::write_int(int n, bool count_band) {
	write((uchar*) &n, 4, count_band);
}

int connection::read_int() {
	int n;
	read((uchar*) &n, 4);
	return n;
}

void connection::write_long(long n, bool count_band) {
	write((uchar*) &n, 8, count_band);
}

long connection::read_long() {
	long n;
	read((uchar*) &n, 8);
	return n;
}
