#include "server.h"

Server::Server()
    : socket_fd(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)), factory(new Factory()), database(new Database()){ 
        if (socket_fd == -1) {
            cerr << "error socket_fd" << endl;
            exit(-1);
        }

        // 2.init server addr 
        struct sockaddr_in bind_addr;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = inet_addr(IP);
        // bind_addr.sin_addr.s_addr = htonl(IP);
        bind_addr.sin_port = htons(PORT);
        if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&bind_addr), sizeof(bind_addr)) == -1) {
            cerr << "bind error" << endl;
            exit(-1);
        }

        // 3.start listen 
        if (listen(socket_fd, SOMAXCONN) == -1) {
            cerr << "listen error" << endl;
            exit(-1);
        }
    }

Server::~Server() {
    close(socket_fd);
    if (factory)
        delete factory;
    if (database)
        delete database;
}


