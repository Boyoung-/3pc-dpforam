#include <iostream>

#include "../util.h"

using namespace std;

int main() {
	int len = 1000;
	uchar* a = new uchar[len];
	uchar* b = new uchar[len];
	uchar* c = new uchar[len];
	cal_xor(a, b, len, c);

	cout << "sccess" << endl;

	return 0;
}
