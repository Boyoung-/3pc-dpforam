#ifndef CONNECTION_H_
#define CONNECTION_H_

class connection {
public:
	virtual ~connection() {
	}
	virtual void init_server(int port) = 0;
	virtual void init_client(const char* ip, int port) = 0;
	virtual void write(const char* data, long bytes) = 0;
	virtual void read(char* data, long bytes) = 0;
	virtual void write_int(int data) = 0;
	virtual int read_int() = 0;
	virtual void close() = 0;
};

#endif /* CONNECTION_H_ */
