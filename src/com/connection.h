#ifndef CONNECTION_H_
#define CONNECTION_H_

class connection {
public:
	virtual ~connection() {
	}
	;
	virtual void write(char* bytes) = 0;
	virtual char* read() = 0;
};

#endif /* CONNECTION_H_ */
