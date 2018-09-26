#include <time.h>
#include <stdint.h>

inline uint64_t getTimeNanoseconds() {
    timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000000000ULL + tv.tv_nsec;
}
