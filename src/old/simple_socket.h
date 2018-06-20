#ifndef SIMPLE_SOCKET_H_
#define SIMPLE_SOCKET_H_

#include <thread>

#include "blocking_queue.h"
#include "connection.h"

class simple_socket: public connection {
private:
	int socket_fd;
	blocking_queue<uchar*> bq;
	std::thread* t1;
public:
	void init_server(int port);
	void init_client(const char* ip, int port);
	void write(const uchar* data, ulong bytes);
	void read(uchar* data, ulong bytes);
	void send(const uchar* data, ulong bytes);
	void recv(uchar* data, ulong bytes);
	void thread_func();
	void close();

	bool flag = true;
};

#endif /* SIMPLE_SOCKET_H_ */
