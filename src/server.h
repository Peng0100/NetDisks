#ifndef __SERVER_H__
#define __SERVER_H__

#include "header.h"
#include "tcp_conn.h"
#include "factory.h"
#include "database.h"

#define IP "127.0.0.1"
#define PORT 3000

class Server{
public:
    Server();

    ~Server();

    inline const int GetSocketFd() const {
        return socket_fd;
    }

    inline Factory* GetFactory() const {
        return factory;
    }

    inline Database* GetDatabase() const {
        return database;
    }

private:
    int     socket_fd;
    
    Factory *factory;
    Database *database;
};

#endif
