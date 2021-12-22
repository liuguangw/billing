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
        bool writeAble= false;
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;
    public:
        explicit TcpConnection(int fd) : connFd(fd) {
            std::cout << "TcpConnection construct fd:" << fd << std::endl;
        }

        ~TcpConnection();

        void setWriteAble(bool s) {
            this->writeAble = s;
        }

        common::IoStatus readAll(unsigned char *buff, size_t nBytes);

        common::IoStatus writeAll();
    };
}


#endif //BILLING_TCP_CONNECTION_H
