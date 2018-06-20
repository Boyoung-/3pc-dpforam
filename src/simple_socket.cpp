#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "simple_socket.h"

void error(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void simple_socket::init_server(int port) {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		error("init_server: socket failed");
	}
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			< 0) {
		error("init_server: setsockopt failed");
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		error("init_server: bind failed");
	}
	if (listen(server_fd, 3) < 0) {
		error("init_server: listen failed");
	}
	int addr_len = sizeof(address);
	socket_fd = accept(server_fd, (struct sockaddr *) &address,
			(socklen_t*) &addr_len);
	if (socket_fd < 0) {
		error("init_server: accept failed");
	}
	set_no_delay();
	::close(server_fd);
}

void simple_socket::init_client(const char* ip, int port) {
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		error("init_client: socket failed");
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
		error("init_client: inet_pton failed");
	}
	if (connect(socket_fd, (struct sockaddr *) &server_addr,
			sizeof(server_addr)) < 0) {
		error("init_client: connect failed");
	}
	set_no_delay();
}

void simple_socket::set_no_delay() {
	int opt = 1;
	if (setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))
			< 0) {
		error("set_no_delay: setsockopt failed");
	}
}

void simple_socket::write(const uchar* data, ulong bytes) {
	long write_bytes;
	ulong offset = 0L;
	while (offset < bytes) {
		write_bytes = ::write(socket_fd, data + offset, bytes - offset);
		if (write_bytes < 0) {
			error("write failed");
		}
		offset += write_bytes;
	}
}

void simple_socket::read(uchar* data, ulong bytes) {
	long read_bytes;
	ulong offset = 0L;
	while (offset < bytes) {
		read_bytes = ::read(socket_fd, data + offset, bytes - offset);
		if (read_bytes < 0) {
			error("read failed");
		}
		offset += read_bytes;
	}
}

void simple_socket::close() {
	::close(socket_fd);
}
