#ifndef CONNECTION_H_
#define CONNECTION_H_

class connection {
public:
	virtual ~connection();

	virtual void init_server(int port) = 0;
	virtual void init_client(const char* ip, int port) = 0;
	virtual void write(const char* data, long bytes) = 0;
	virtual void read(char* data, long bytes) = 0;
	virtual void close() = 0;

	void write_int(int n);
	int read_int();
};

#endif /* CONNECTION_H_ */
