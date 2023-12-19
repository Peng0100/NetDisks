#include "factory.h"
#include "server.h"
#include "time.h"

extern Server server;

Factory::Factory() {
    pthread_cond_init(&cond, nullptr);
    pthread_mutex_init(&mutex, nullptr);		

    for (int i = 0; i < THREAD_NUM; ++i) {
        Thread *thread = new Thread(); 
        threads.push_back(thread);
    }
}

Factory::~Factory() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    for (auto elem : threads) 
        delete elem;

    while (!remainder_conn.empty()) {
        auto elem = remainder_conn.front();
        remainder_conn.pop();
        delete elem;
    }
}

void Factory::DispatchConn(TcpConn *conn) {
    for (auto elem : threads) {
        if (!elem->Busy()) {
            elem->DispatchConn(conn);
            return ;
        }
    }

    RemainderConnIn(conn);
}

void Factory::RemainderConnIn(TcpConn *conn) {
    pthread_mutex_lock(&mutex);
    // share memory
    remainder_conn.push(conn);
    //pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

TcpConn* Factory::RemainderConnOut(struct timespec t) {
    TcpConn *conn = nullptr;

    if (pthread_mutex_timedlock(&mutex, &t) != 0) {
        //pthread_mutex_lock(&mutex);
        if (!remainder_conn.empty()){
            conn = remainder_conn.front();
            remainder_conn.pop();
        }

        pthread_mutex_unlock(&mutex);
    }

    return conn; 
}

/*-----------------------Thread--------------------------------*/

Thread::Thread() 
    :pthread_id(-1), epoll_fd(epoll_create(1)), busy(false) {
        pthread_create(&pthread_id, nullptr, PThreadFunc, this);
        // cout << "pthread_id :" << pthread_id << "start..." << endl;
    }

Thread::~Thread() {
    pthread_cancel(pthread_id);

    long thread_ret;
    pthread_join(pthread_id, (void**)&thread_ret);

    for (auto elem : conns)
        delete elem.second;
}


void Thread::DispatchConn(TcpConn* conn) {
    // 1.epoll add read
    struct epoll_event ev;
    ev.data.fd = conn->GetConnFd();
    ev.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn->GetConnFd(), &ev) == -1) {
        cerr << "epoll_ctl error" << endl;
    }

    // 2.push 
    conns[conn->GetConnFd()] = conn;
}

void* PThreadFunc(void *arg) {
    auto thread_info = static_cast<Thread*>(arg);
    struct epoll_event events[thread_info->EVENTMAXSIZE];

    Timer timer;
    while (1) {
        timer.Start();
        timer.Check();

        // 1.epoll_wait
        int ret = epoll_wait(thread_info->epoll_fd, events, thread_info->EVENTMAXSIZE, 0);
        if (ret == -1) {
            if (errno == EINTR) 
                continue;

            break;
        } 

        // 2.i/o task
        for (int i = 0; i < ret; ++i) {
            auto conn = thread_info->conns[events[i].data.fd];

            int io_ret = 0;
            if (events[i].events == EPOLLIN) {
                io_ret = conn->Recv();
            } else if (events[i].events == EPOLLOUT) {
                io_ret = conn->Send();
                epoll_ctl(thread_info->epoll_fd, EPOLL_CTL_DEL, conn->GetConnFd(), 0);
            } else {
                io_ret = -1;
            }

            if (io_ret == -1) {
                // client error 
                thread_info->conns.erase(conn->GetConnFd());
                delete conn;
            } else {
                thread_info->cpu_task.push(conn);
            }
        }

        // 3.cpu task
        while (!thread_info->cpu_task.empty() && !timer.Timeout()) {
            auto conn = thread_info->cpu_task.front();
            thread_info->cpu_task.pop();
            if (conn) {
                int handle_ret = conn->Handle();
                if (handle_ret == -1) {
                    thread_info->conns.erase(conn->GetConnFd());
                    delete conn;
                    continue;
                }

                struct epoll_event ev; 
                ev.data.fd = conn->GetConnFd();
                ev.events = EPOLLOUT;
                if (epoll_ctl(thread_info->epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
                    perror("epoll_ctl");
                    thread_info->conns.erase(conn->GetConnFd());
                    delete conn;
                    continue;
                }
            }
        }

        if (timer.Timeout()) {
            thread_info->busy = true;
            continue;
        } 
        thread_info->busy = false;

        // if time is ok 
        // 1.check conn timer is ok
        // 2.get new conn
        cout << "child thread : " << thread_info->pthread_id << " conns : " << thread_info->conns.size() << endl; 
        TcpConn *conn = server.GetFactory()->RemainderConnOut(timer.RemainderTime());
        if (conn) {
            thread_info->conns[conn->GetConnFd()] = conn;
        }
    }

    return nullptr;
}

