#include <unistd.h>
#include <fcntl.h>
#include <string.h>

inline int setValue(const char *path, const char *data) {
	int fd = open(path, O_WRONLY);
	if (fd<0) return fd;
	int n = write(fd,  data , strlen(data));
	close(fd);
	return n;
}
