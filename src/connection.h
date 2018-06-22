#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "typedef.h"

class connection {
public:
	ulong bandwidth = 0u;

	virtual ~connection();
	virtual void init_server(int port) = 0;
	virtual void init_client(const char* ip, int port) = 0;
	virtual void set_no_delay() = 0;
	virtual void write(const uchar* data, ulong bytes,
			bool count_band = true) = 0;
	virtual void read(uchar* data, ulong bytes) = 0;
	virtual void fwrite(const uchar* data, ulong bytes,
			bool count_band = true) = 0;
	virtual void fread(uchar* data, ulong bytes) = 0;
	virtual void flush() = 0;
	virtual void close() = 0;
	void write_int(int n, bool count_band = true);
	int read_int();
	void write_long(long n, bool count_band = true);
	long read_long();
};

#endif /* CONNECTION_H_ */
