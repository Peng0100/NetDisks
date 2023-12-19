#ifndef __TCPCONN_H__
#define __TCPCONN_H__

#include "header.h"
#include "channel.h"

class TcpConn {
public:
    TcpConn(int client_fd, void *client_info);
    ~TcpConn();

    int GetConnFd() const { return client_fd; }
    int Recv();
    int Send();
    int Handle();

private:
    pair<void*, size_t> __Handle(void*);
    /* log : 0
     * ls : 1 cd : 2 mkdir : 3 remove : 4 
     * send : 5 recv : 6
     * quit : 7
     * Confirm : 8
     * */
    int status;
    const vector<map<size_t, size_t>> procedure = {
        {{0, 1}},
        {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 2}, {6, 2}, {7, 3}},
        {{5, 2}, {6, 2}}
    };

    int client_fd;
    struct sockaddr_in client_info;

    Channel *channel;
};

#endif
