#include "channel.h"

Channel::Channel() {
    recv_buf_size = send_buf_szie = BUF_INIT_SIZE;

    recv_buf = malloc(recv_buf_size);
    if (recv_buf == nullptr)
        cerr << "recv buf init malloc fail" << endl;
    recv_buf_head = recv_buf_tail = recv_buf;

    send_buf = malloc(send_buf_szie);
    if (send_buf == nullptr)
        cerr << "send buf init malloc fail" << endl;
    send_buf_head = send_buf_tail = send_buf;
}

Channel::~Channel() {
    if (recv_buf)
        free(recv_buf);
    recv_buf = nullptr;
    recv_buf_head = recv_buf_tail = nullptr;

    if (send_buf)
        free(send_buf);
    send_buf = nullptr;
    send_buf_head = send_buf_tail = nullptr;
}

int Channel::RecvBufSizeCheck(const size_t len) {
    if ((char*)recv_buf + recv_buf_size < (char*)recv_buf_tail + len) {
        size_t data_size = (char*)recv_buf_tail - (char*)recv_buf_head;
        memcpy(recv_buf, recv_buf_head, data_size);

        if (recv_buf_size - data_size < len) {
            recv_buf_size = 2 * len;
            if (recv_buf_size > BUF_MAX_SIZE) {
                cerr << "recv buf size too large" << endl;
                return -1;
            }

            recv_buf = realloc(recv_buf, recv_buf_size);
            if (recv_buf == nullptr) {
                cerr << "realloc error" << endl;
                return -1;
            }
        }
    }

    return 0;
}

int Channel::RecvCycle(int fd, void *buf, off_t len){
    int recv_len = 0, intr_times = 3;

    while (recv_len < len) {
        int ret = recv(fd, static_cast<char*>(buf) + recv_len, len - recv_len, 0);
        if (ret == 0) {
            cerr << "client shutdown" << endl;
            return recv_len;
        } else if (ret < 0) {
            if (errno == EINTR && intr_times--) 
                continue;

            perror("recv");
            return -1;
        }

        recv_len += ret;
    }

    return recv_len;
}


int Channel::Recv(int fd) {
    size_t data_size;
    if (RecvBufSizeCheck(sizeof(data_size)) == -1)
        return -1;
    if (sizeof(data_size) != RecvCycle(fd, &data_size, sizeof(data_size))) 
        return -1;    
    memcpy(recv_buf_tail, &data_size, sizeof(data_size));
    recv_buf_tail = (char*)recv_buf_tail + sizeof(data_size);

    if (RecvBufSizeCheck(data_size) == -1)
        return -1;
    if (data_size != RecvCycle(fd, recv_buf_tail, data_size)) 
        return -1;    
    recv_buf_tail = (char*)recv_buf_tail + data_size;

    return 0;
}

int Channel::SendCycle(int fd, void *buf, off_t len){
    int send_len = 0, intr_times = 3;

    while (send_len < len) {
        int ret = send(fd, static_cast<char*>(buf) + send_len, len - send_len, 0);
        if (ret == 0) {
            return send_len;
        } else if (ret < 0) {
            if (errno == EINTR && intr_times--) 
                continue;

            perror("send");
            return -1;
        }

        send_len += ret;
    }

    return send_len;
}


int Channel::Send(int fd) {
    size_t data_size;
    if ((char*)send_buf_tail - (char*)send_buf_head < 2 * sizeof(data_size)) {
        cerr << "no data to send" << endl;
        return -1;
    }

    data_size = reinterpret_cast<size_t>(send_buf_head);
    if (sizeof(data_size) != SendCycle(fd, send_buf_head, sizeof(data_size))) 
        return -1;
    send_buf_head = (char*)send_buf_head + sizeof(data_size);

    if (data_size != SendCycle(fd, send_buf_head, data_size))
        return -1;
    send_buf_head = (char*)send_buf_head + data_size;

    return 0;
}

int Channel::Get(size_t *request, void *data, size_t *data_size) {
    if ((char*)recv_buf_tail - (char*)recv_buf_head < sizeof(size_t) * 2) {
        cerr << "no date to get" << endl;
        return -1;
    }

    // real data size not include request
    *data_size = reinterpret_cast<size_t>(recv_buf_head) - sizeof(size_t);
    recv_buf_head = (char*)recv_buf_head + sizeof(size_t);

    *request = reinterpret_cast<size_t>(recv_buf_head);
    recv_buf_head = (char*)recv_buf_head + *data_size;

    data = recv_buf_head;

    return 0;
}

int Channel::Put(size_t request, void *data, size_t data_size) {
    // data_size + request + data
    if (RecvBufSizeCheck(data_size + 2 * sizeof(data_size)) == -1)
        return -1;

    data_size += sizeof(request);
    memcpy(send_buf_tail, &data_size, sizeof(data_size));
    send_buf_tail = (char*)send_buf_tail + sizeof(data_size);

    memcpy(send_buf_tail, data, data_size);
    send_buf_tail = (char*)send_buf_tail + data_size;

    return 0;
}

