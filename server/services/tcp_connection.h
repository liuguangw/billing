//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_TCP_CONNECTION_H
#define BILLING_TCP_CONNECTION_H

#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include "../common/io_status.h"
#include <pthread.h>

namespace services {
    void* processTcpData(void* arg);
    class TcpConnection {
    private:
        int connFd;
        bool writeAble = false;
        pthread_t threadT = 0;
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;
        void processData();
    public:
        explicit TcpConnection(int fd) : connFd(fd) {
            std::cout << "TcpConnection construct fd:" << fd << std::endl;
        }

        ~TcpConnection();

        void setWriteAble(bool s) {
            this->writeAble = s;
        }

        void startWorkingThread();

        common::IoStatus readAll(unsigned char *buff, size_t nBytes);

        common::IoStatus writeAll();
        friend void* processTcpData(void* arg);
    };
}


#endif //BILLING_TCP_CONNECTION_H
