#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>		//Basic thread library

#include "create_socket.h"
#include "Socket.h"
#include "Setup.h"

static void *socket_func(void *arg){
    socket_input_t *client = (socket_input_t *)arg;
    
    // Open the INET Socket for writing.
	printf("Connecting to the Socket...\n");
	client->fd = create_socket(0, client->IP);
	if (client->fd <0) exit(0);
	
	while (1) {

		// Reads the payload
		client->sz = read(client->fd, client->cmd, sizeof(client->cmd));
		if (client->sz == -1) break;
		else if (client->sz == 0) {
			printf("Server Lost!\n");
			break;
		}
		
		// Place the PWM signal on the correspondent PWM variable
		if(client->cmd[0]>=0) {	
			client->LIA = client->cmd[0];	
			client->LIB = 0;	
		}
		else {	
			client->LIA = 0;	
			client->LIB = (-1)*client->cmd[0];	
		}
		if(client->cmd[1]>=0) {	
			client->RIA = client->cmd[1];	
			client->RIB = 0;	
		}
		else {
			client->RIA = 0;	
			client->RIB = (-1)*client->cmd[1];	
		}
	}
	
	return 0;
}

socket_input_t *client_create(const char *IP, pthread_mutex_t *mutex) {
    socket_input_t *sc = new socket_input_t();
    
    sc->IP = IP;
    sc->mutex = mutex;
    sc->gameOver = false;
    
    pthread_create(&sc->thread_id, NULL, socket_func, sc);

    return sc;
}