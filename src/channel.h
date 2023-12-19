#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "header.h"

#define BUF_INIT_SIZE (1 << 10)
#define BUF_MAX_SIZE (1 << 24)

class Channel {
public:
    Channel();
    ~Channel();

    int RecvBufSizeCheck(const size_t len);
    int Recv(int fd); 
    int Send(int fd);

    int Get(size_t *request, void *data, size_t *data_size);
    int Put(const size_t request, void *data, const size_t data_size);

private:
    int RecvCycle(int fd, void *buf, off_t len);
    int SendCycle(int fd, void *buf, off_t len);


    size_t recv_buf_size;
    void    *recv_buf;
    void    *recv_buf_head;
    void    *recv_buf_tail;

    size_t send_buf_szie;
    void    *send_buf;
    void    *send_buf_head;
    void    *send_buf_tail;
};

#endif
