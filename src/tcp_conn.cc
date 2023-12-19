#include "tcp_conn.h"
#include "server.h"

extern Server server;

TcpConn::TcpConn(int _client_fd, void *_client_info)
    :client_fd(_client_fd), status(0), channel(new Channel()){
        memcpy(&client_info, _client_info, sizeof(struct sockaddr_in)); 
    }

TcpConn::~TcpConn() {
    close(client_fd);
    delete channel;
}

int TcpConn::Recv() {
    return channel->Recv(client_fd);
}

int TcpConn::Send() {
    return channel->Send(client_fd);
}

int TcpConn::Handle() {
    char *data;
    size_t request, data_size;
    if (channel->Get(&request, data, &data_size) == -1) {
        return -1;
    }

    // 1. check 
    // check status
    auto p_map = procedure[status];
    if (p_map.find(request) == p_map.end()) 
        return -1;

    // check data
    auto ret = __Handle(data);
    if (ret.second == 0) {
        return -1;
    }

    // 2. response
    channel->Put(status, ret.first, ret.second);
    free(ret.first);
    // epoll ctl epoll in
   
    // 3. change status
    status = p_map[request];
    if (status == 3) 
        return -1;

    return 0;
}

pair<void*, size_t> TcpConn::__Handle(void *data) {
    auto p_database = server.GetDatabase();

    if (status == 0) {
        return p_database->Log1(data);
    } else if (status == 1) {
        return p_database->Ls(data);
    } else if (1) {

    }

    return make_pair(nullptr, 0);
}

