#ifndef SIMPLE_SOCKET_H_
#define SIMPLE_SOCKET_H_

#include "connection.h"

class simple_socket: public connection {
private:
	int socket_fd;
public:
	void init_server(int port);
	void init_client(const char* ip, int port);
	void close();
	void write(const char* bytes, long size);
	long read(char* bytes);
};

#endif /* SIMPLE_SOCKET_H_ */
