#ifndef CONNECTION_H_
#define CONNECTION_H_

class connection {
public:
	virtual ~connection() {
	}
	virtual void init_server(int port) = 0;
	virtual void init_client(const char* ip, int port) = 0;
	virtual void close() = 0;
	virtual void write(const char* bytes, long size) = 0;
	virtual long read(char* bytes) = 0;
};

#endif /* CONNECTION_H_ */
