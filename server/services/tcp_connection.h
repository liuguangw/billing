//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_TCP_CONNECTION_H
#define BILLING_TCP_CONNECTION_H

#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include "../common/io_status.h"

namespace services {
    class TcpConnection {
    private:
        int connFd;
        bool writeAble;
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;
    public:
        TcpConnection() : connFd(-1) {
            std::cout << "TcpConnection construct" << std::endl;
        }

        ~TcpConnection();

        inline int getConnFd() {
            return this->connFd;
        }

        void initConnection(int epollFd, int serverSockFd, epoll_event *epollEvent);

        void setWriteAble(bool s) {
            this->writeAble = s;
        }

        common::IoStatus readAll(unsigned char *buff, size_t nBytes);

        common::IoStatus writeAll();
    };
}


#endif //BILLING_TCP_CONNECTION_H
