#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "simple_socket2.h"

void error2(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void simple_socket2::init_server(int port) {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		error2("init_server: socket failed");
	}
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			< 0) {
		error2("init_server: setsockopt failed");
	}
	opt = 1;
	if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))
			< 0) {
		error2("init_server: TCP_NODELAY failed");
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		error2("init_server: bind failed");
	}
	if (listen(server_fd, 1) < 0) {
		error2("init_server: listen failed");
	}
	int addr_len = sizeof(address);
	socket_fd = accept(server_fd, (struct sockaddr *) &address,
			(socklen_t*) &addr_len);
	if (socket_fd < 0) {
		error2("init_server: accept failed");
	}
	//::close(server_fd);

	stream = fdopen(socket_fd, "wb+");
	buffer = new char[1000];
	memset(buffer, 0, 1000);
	setvbuf(stream, buffer, _IOFBF, 1000);
}

void simple_socket2::init_client(const char* ip, int port) {
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		error2("init_client: socket failed");
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
		error2("init_client: inet_pton failed");
	}
	if (connect(socket_fd, (struct sockaddr *) &server_addr,
			sizeof(server_addr)) < 0) {
		error2("init_client: connect failed");
	}

	stream = fdopen(socket_fd, "wb+");
	buffer = new char[1000];
	memset(buffer, 0, 1000);
	setvbuf(stream, buffer, _IOFBF, 1000);
}

#include <iostream>

void simple_socket2::write(const uchar* data, ulong bytes) {
	//std::cout << "start of write" << std::endl;
	long write_bytes;
	ulong offset = 0L;
	while (offset < bytes) {
		write_bytes = fwrite(data + offset, 1, bytes - offset, stream);
		if (write_bytes < 0) {
			error2("write failed");
		}
		offset += write_bytes;
		//fflush(stream);
		//std::cout << "write " << offset << " " << bytes << std::endl;
	}
	fflush(stream);
	//std::cout << "end of write" << std::endl;
}

void simple_socket2::read(uchar* data, ulong bytes) {
	//std::cout << "start of read" << std::endl;
	long read_bytes;
	ulong offset = 0L;
	while (offset < bytes) {
		//std::cout << "before read " << offset << " " << bytes << std::endl;
		read_bytes = fread(data + offset, 1, bytes - offset, stream);
		if (read_bytes < 0) {
			error2("read failed");
		}
		offset += read_bytes;
		//std::cout << "after read " << offset << " " << bytes << std::endl;
	}
	//std::cout << "end of read" << std::endl;
	//fflush(stream);
}

void simple_socket2::close() {
	::close(socket_fd);
}
