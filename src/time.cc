#include "time.h"

bool Timer::Timeout() const {
    struct timespec now_time;
    clock_gettime(CLOCK_REALTIME, &now_time);

    long diff = (now_time.tv_sec - old_time.tv_sec) * NSECTOSEC;
    diff += now_time.tv_nsec - old_time.tv_nsec;
    return diff >= timeout;
}

struct timespec Timer::RemainderTime() {
    struct timespec end_time;
    end_time.tv_nsec = old_time.tv_nsec + timeout % NSECTOSEC;
    end_time.tv_sec = old_time.tv_sec + timeout / NSECTOSEC + end_time.tv_nsec / NSECTOSEC;
    end_time.tv_nsec = end_time.tv_nsec % NSECTOSEC;

    return end_time;
}
