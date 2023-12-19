#include "header.h"
#include "server.h"
#include "time.h"

Server server;

int main(int argc, char **argv) {
    int socket_fd = server.GetSocketFd();
    auto factory = server.GetFactory();

    while (true) {
        Timer timer;

        for (int i = 0; i < 3;) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(socket_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
            if (client_fd == -1) {
                if (errno = EWOULDBLOCK) {
                    cout << "server no connect" << endl;
                    sleep(2);
                    break;
                } else if(errno == EINTR) {
                    continue;
                } else {
                    perror("accept");
                    exit(0);
                }
            }

            cout << "new conn, ip : " << client_addr.sin_addr.s_addr << "port : " 
                << ntohs(client_addr.sin_port) << endl;

            factory->DispatchConn(new TcpConn(client_fd, &client_addr));
            ++i;
        } 

        if (timer.Timeout()) {
            continue;
        }

        // can to do other thing
        cout << "main thred nothing to do..." << endl;
        sleep(1);
    }

    return 0;
}
