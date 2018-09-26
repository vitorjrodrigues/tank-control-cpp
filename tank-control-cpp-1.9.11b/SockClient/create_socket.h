#ifndef CREATE_SOCKET_H
#define CREATE_SOCKET_H

struct socket_client {
	int sock;
	ssize_t sz;
};

int create_socket(int server, const char *addr);

#endif
