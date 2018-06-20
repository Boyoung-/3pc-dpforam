#include <iostream>

#include "simple_socket.h"

using namespace std;

int main(int argc, const char* argv[]) {
	simple_socket cons[2];
	uchar t[4096];
	if (argc == 1) {
		cons[0].init_server(8000);
		cons[1].init_server(8001);
	} else if (argc == 2) {
		cons[1].init_client("127.0.0.1", 8000);
		cons[0].init_server(8002);
	} else {
		cons[0].init_client("127.0.0.1", 8001);
		cons[1].init_client("127.0.0.1", 8002);
	}

	for (int i=0; i<100000; i++) {
		cout << i << endl;
		cons[0].write(t, 4096);
		cons[1].write(t, 4096);
		cons[0].read(t, 4096);
		cons[1].read(t, 4096);
	}


	return 0;
}
