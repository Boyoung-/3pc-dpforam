#include <iostream>
#include <string.h>

#include "connection.h"
#include "simple_socket.h"

using namespace std;

int main() {
	connection* con = new simple_socket();
	con->init_client("127.0.0.1", 8000);

	const char* send = "hello from client";
	con->write(send, strlen(send));

	char recv[1024];
	con->read(recv);
	cout << recv << endl;

	con->close();
	return 0;
}
