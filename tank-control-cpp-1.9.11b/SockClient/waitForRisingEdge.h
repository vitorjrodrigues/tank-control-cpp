#include <poll.h>
#include <unistd.h>

inline int waitForRisingEdge(int fd_value) {
    pollfd pfd[1];
    pfd[0].fd = fd_value;
    pfd[0].events = POLLPRI | POLLERR;
    pfd[0].revents = 0;

    lseek(fd_value, 0, SEEK_SET);
    char dummy[16];
    read(fd_value, dummy, 16);

    return poll(pfd, 1, 1000);
}
