#ifndef SOCKET_H
#define SOCKET_H

struct socket_input_t {
    pthread_t thread_id;
    pthread_mutex_t *mutex;
    const char *IP;
	int fd;
	ssize_t sz;
	int cmd[2];
	int cmd_l;
	int cmd_r;
	int LIA;
	int LIB;
	int RIA;
	int RIB;
	bool gameOver;
};

socket_input_t *client_create(const char *IP, pthread_mutex_t *mutex);

#endif