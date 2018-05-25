#include <iostream>
#include <string.h>

#include "connection.h"
#include "simple_socket.h"

using namespace std;

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		cout << "Usage: ui [party]" << endl;
		return 0;
	}

	int port = 8000;
	const char* host = "127.0.0.1";
	connection* cons[2];
	cons[0] = new simple_socket();
	cons[1] = new simple_socket();

	if (strcmp(argv[1], "eddie") == 0) {
		cout << "Establishing connection with debbie... " << flush;
		cons[0]->init_server(port);
		cout << "done" << endl;

		cout << "Establishing connection with charlie... " << flush;
		cons[1]->init_server(port + 1);
		cout << "done" << endl;

	} else if (strcmp(argv[1], "debbie") == 0) {
		cout << "Connecting with eddie... " << flush;
		cons[1]->init_client(host, port);
		cout << "done" << endl;

		cout << "Establishing connection with charlie... " << flush;
		cons[0]->init_server(port + 2);
		cout << "done" << endl;

	} else if (strcmp(argv[1], "charlie") == 0) {
		cout << "Connecting with eddie... " << flush;
		cons[0]->init_client(host, port + 1);
		cout << "done" << endl;

		cout << "Connecting with debbie... " << flush;
		cons[1]->init_client(host, port + 2);
		cout << "done" << endl;

	} else {
		cout << "Incorrect party: " << argv[1] << endl;
	}

	cout << "Closing connections... " << flush;
	delete cons[0];
	delete cons[1];
	cout << "done" << endl;

	return 0;
}
