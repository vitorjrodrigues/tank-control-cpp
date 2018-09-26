#include <iostream>
#include <pthread.h>
#include "speedFeedback.h"
#include "getTimeNanoseconds.h"
#include "waitForRisingEdge.h"
#include "sysfsHelper.h"

using namespace std;

static void *feedback_func(void *vpArg) {
    feedback_info_t *fi = (feedback_info_t *)vpArg;
    
    // Setup Feedback Pins
    char buff[128];
    snprintf(buff, 128, "%d", fi->gpio);
    setValue("/sys/class/gpio/export", buff);
    
    snprintf(buff, 128, "/sys/class/gpio/gpio%d/direction", fi->gpio);
    if (setValue(buff, "in") < 0) exit(0);

    snprintf(buff, 128, "/sys/class/gpio/gpio%d/edge", fi->gpio);
    if (setValue(buff, "rising") < 0) exit(0);
    
    snprintf(buff, 128, "/sys/class/gpio/gpio%d/value", fi->gpio);
    int fd_value = open(buff, O_RDWR);
    if (fd_value < 0) exit(0);

    // Wait for a rising edge
    waitForRisingEdge(fd_value);
    
    // Get "old" time 
    uint64_t old_time = getTimeNanoseconds();
    
    // 
    while (true) {
        // Wait for a rising edge
        int r = waitForRisingEdge(fd_value);
        
        // Give up on wait error
        if (r < 0)
            break;
        
        // if desligar? : desligar!
        if (fi->gameOver)
            break;
        
        // Ignore timeouts
        if (r == 0) {
            pthread_mutex_lock(fi->mutex);
            fi->speed = 0;
            pthread_mutex_unlock(fi->mutex);
            continue;
        }

        // Get current time 
        uint64_t new_time = getTimeNanoseconds();
        
        // Compute period
        double period = (new_time - old_time) * 1e-9;

        // Update old time
        old_time = new_time;
        
        // Update status
        pthread_mutex_lock(fi->mutex);
        if (*fi->command >= 0) {
            fi->pos += 1;
            fi->speed = +1/period;
        } else {
            fi->pos -= 1;
            fi->speed = -1/period;
        }
        
        pthread_mutex_unlock(fi->mutex);
    }
    
    return 0;
}

feedback_info_t *feedback_create(int gpio, double *command, pthread_mutex_t *mutex) {
    feedback_info_t *fi = new feedback_info_t();
    
    fi->gpio = gpio;
    fi->pos = 0;
    fi->speed = 0;
    fi->command = command;
    fi->mutex = mutex;
    fi->gameOver = false;
    
    pthread_create(&fi->thread_id, NULL, feedback_func, fi); //cria a thread "feedback" passando a função como argumento

    return fi;
}
