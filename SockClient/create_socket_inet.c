#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKET_NAME "/tmp/our_unix_socket"

// Server creation: socket, bind(sockaddr_in), listen, accept
// Client creation: socket, connect(sockaddr_in)

int create_socket(int server, const char *addr) {
	// Create a UNIX-domain socket.
	printf("Creating socket...\n");
	int s = socket(AF_INET, SOCK_STREAM, 0);

	// socket(...) returns -1 on error.
	if (s == -1) {
		printf("socket(...) failed!\n");
		return -1;
	}

	// Internet sockets have an IP address and a port number.
	// AF_INET uses IPv4, so address is a 32-bit number, usually expressed as
	// byte.byte.byte.byte with each byte in decimal. For instance: 192.168.0.1
	// Port number is 16bit, 0-65535, and must be the same on server and client.
	struct sockaddr_in sa;
	sa.sin_family  = AF_INET; // Mandatory
	sa.sin_port    = htons(8080);
	if (addr) sa.sin_addr.s_addr = inet_addr(addr);

	// Server and client socket creation is slightly different...
	int r;
	if (server) {
		// On the server side the address field selects a network card.
		if (!addr) sa.sin_addr.s_addr = INADDR_ANY;

		// Now we bind(...) the socket to an address, and
		// later we will accept(...) connections from it.
		r = bind(s, (struct sockaddr *)&sa, sizeof(sa));

		// Bind may fail and return -1. If it does not fail,
		if (r != -1) {
			// then we tell the socket to start listening for clients.
			r = listen(s, 10);
		}
	} else {
		// On the client we set the address of the server to connect to.
		if (!addr) sa.sin_addr.s_addr = inet_addr("127.0.0.1");

		// We connect(...) a client socket to an address.
		r = connect(s, (struct sockaddr *)&sa, sizeof(sa));
	}

	// Both bind, listen and connect all return -1 on error.
	if (r == -1) {
		// A failed socket is useless, close it.
		close(s);

		printf("bind, listen or or connect failed!\n");
		return -1;
	}

	// Server socket is now fully configured.
	printf("Socket ready.\n");
	return s;
}
