//
// Created by liuguang on 2021/12/21.
//

#include "tcp_connection.h"
#include "../common/billing_exception.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>

namespace services {
    using common::IoStatus;

    TcpConnection::~TcpConnection() {
        close(this->connFd);
        std::cout << "TcpConnection destroy fd:" << this->connFd << std::endl;
    }

    IoStatus TcpConnection::readAll(unsigned char *buff, size_t nBytes) {
        ssize_t readCount;
        while (true) {
            readCount = read(connFd, buff, nBytes);
            std::cout << "readCount: " << readCount << std::endl;
            if (readCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    return IoStatus::Ok;
                } else {
                    return IoStatus::Error;
                }
            } else if (readCount == 0) {
                return IoStatus::Disconnected;
            }
            this->inputData.insert(this->inputData.end(), buff, buff + readCount);
            std::cout << "====================================================" << std::endl;
            for (std::size_t i = 0; i < this->inputData.size(); i++) {
                std::cout << std::hex << (int) this->inputData[i] << " ";
                if ((i % 16 == 15) || (i == this->inputData.size() - 1)) {
                    std::cout << std::endl;
                }
            }
            std::cout << "====================================================" << std::endl;
            return IoStatus::Ok;
        }
    }
}