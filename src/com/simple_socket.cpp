#include <iostream>

#include "connection.h"
#include "simple_socket.h"

void simple_socket::write(char* bytes) {
	std::cout << "write works" << std::endl;
}

char* simple_socket::read() {
	std::cout << "read works" << std::endl;
	return NULL;
}

int main() {
	std::cout << "hello" << std::endl;
	simple_socket my_socket;
	connection* con = &my_socket;
	con->write(NULL);
	con->read();
	return 0;
}
