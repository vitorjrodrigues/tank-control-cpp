#ifndef SPEEDFEEDBACK_H
#define SPEEDFEEDBACK_H

struct feedback_info_t {
    pthread_t thread_id;
    pthread_mutex_t *mutex;
    int gpio;
    double *command;
    int pos;
    double speed;
    bool gameOver;
};

feedback_info_t *feedback_create(int gpio, double *command, pthread_mutex_t *mutex);

#endif
