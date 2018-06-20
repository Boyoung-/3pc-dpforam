#ifndef SIMPLE_SOCKET2_H_
#define SIMPLE_SOCKET2_H_

#include "connection.h"

class simple_socket2: public connection {
private:
	int socket_fd;
	FILE* stream;
	char* buffer;
public:
	void init_server(int port);
	void init_client(const char* ip, int port);
	void write(const uchar* data, ulong bytes);
	void read(uchar* data, ulong bytes);
	void close();
};

#endif /* SIMPLE_SOCKET2_H_ */
