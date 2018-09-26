#ifndef SOCKET_H
#define SOCKET_H

struct socket_input_t {
    pthread_t thread_id;
    pthread_mutex_t *mutex;
    const char *IP;
	int fd;
	ssize_t sz;
	int cmd[2];
	double cmd_l;
	double cmd_r;
	bool gameOver;
};

socket_input_t *client_create(const char *IP, pthread_mutex_t *mutex);

#endif