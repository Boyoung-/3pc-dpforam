#ifndef SIMPLE_SOCKET_H_
#define SIMPLE_SOCKET_H_

#include "connection.h"

class simple_socket: public connection {
private:
	int socket_fd;
	FILE* stream;
	char* buffer;

	void set_stream();
public:
	void init_server(int port);
	void init_client(const char* ip, int port);
	void set_no_delay();
	void write(const uchar* data, ulong bytes);
	void read(uchar* data, ulong bytes);
	void fwrite(const uchar* data, ulong bytes);
	void fread(uchar* data, ulong bytes);
	void flush();
	void close();
};

#endif /* SIMPLE_SOCKET_H_ */
