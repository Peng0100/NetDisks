#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "header.h"
#include "tcp_conn.h"
#include <pthread.h>

#define THREAD_NUM 3


#define ThreadErrorCheck(ret, fun_name) {\
    if (ret) {\
        cerr << fun_name << strerr(ret) << endl;\
    }\
}

void* PThreadFunc(void *arg);

class Thread{
public:
    Thread(); 
    ~Thread();

    inline bool Busy() const { return busy; }
    inline int PThreadId() const { return pthread_id; }

    void DispatchConn(TcpConn* data);

private:
    friend void* PThreadFunc(void*);

    const int EVENTMAXSIZE = 1024;

    pthread_t pthread_id;
    int epoll_fd;
    bool busy;
    // pthread_mutex_t mutex;
    map<int, TcpConn*> conns;
    queue<TcpConn*> cpu_task;
};

class Factory{
public:
    Factory();
    ~Factory();

    void DispatchConn(TcpConn*);
    void RemainderConnIn(TcpConn*);
    TcpConn* RemainderConnOut(struct timespec);

private:
    queue<TcpConn*>    remainder_conn;
    vector<Thread*>     threads;

    pthread_cond_t	    cond;
    pthread_mutex_t 	mutex;
};

#endif
