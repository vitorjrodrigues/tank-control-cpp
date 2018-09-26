#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>		//Basic thread library

#include "create_socket.h"
#include "Socket.h"
#include "Setup.h"

static void *socket_func(void *arg){
    socket_input_t *sc = (socket_input_t *)arg;
    
    // Open the INET Socket for writing.
	sc->fd = create_socket(0, sc->IP);
	if (sc->fd <0) exit(0);
	
	while (1) {
		
		// Reads the payload
		sc->sz = read(sc->fd, sc->cmd, sizeof(sc->cmd));
		if (sc->sz == -1) break;
		else if (sc->sz == 0) {
			printf("Server Lost!\n");
			break;
		}
		
		// Lock Mutex to read current event and update values
		pthread_mutex_lock(sc->mutex);
		
		sc->cmd_l = (double)(sc->cmd[0]);
		sc->cmd_r = (double)(sc->cmd[1]);
		
		// Unlock Mutex
		pthread_mutex_unlock(sc->mutex);
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