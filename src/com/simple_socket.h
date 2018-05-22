#ifndef SIMPLE_SOCKET_H_
#define SIMPLE_SOCKET_H_

#include "connection.h"

class simple_socket: public connection {
public:
	void write(char* bytes);
	char* read();
};

#endif /* SIMPLE_SOCKET_H_ */
