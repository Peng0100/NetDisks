#ifndef __VRDRIVE_TIME_H__
#define __VRDRIVE_TIME_H__

#include "header.h"
#include <time.h>
#include <sys/time.h>

#define NSECTOSEC 1000000000

class Timer{
public:
    Timer(const long timeout = 2000000000)
        : timeout(timeout) {
            clock_gettime(CLOCK_REALTIME, &old_time);
        }

    bool Timeout() const;

    void Start() const;
    void Check() const;

    struct timespec RemainderTime();

private:
    vector<pair<struct timespec, TcpConn*>> timer_conns;

    long timeout;

    struct timespec old_time;
};


#endif
