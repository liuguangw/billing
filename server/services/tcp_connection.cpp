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
        if (this->connFd >= 0) {
            close(this->connFd);
            std::cout << "TcpConnection close fd:" << this->connFd << std::endl;
        }
        std::cout << "TcpConnection destroy" << std::endl;
    }

    void TcpConnection::initConnection(int epollFd, int serverSockFd, epoll_event *epollEvent) {
        this->connFd = accept4(serverSockFd,
                               (struct sockaddr *) nullptr, nullptr, SOCK_NONBLOCK);
        if (this->connFd < 0) {
            throw common::BillingException("accept connection failed", errno);
        }
        epollEvent->events = EPOLLIN | EPOLLOUT | EPOLLET;
        epollEvent->data.fd = this->connFd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, this->connFd, epollEvent) == -1) {
            throw common::BillingException("epoll register connection socket failed", errno);
        }
    }

    IoStatus TcpConnection::readAll(unsigned char *buff, size_t nBytes) {
        ssize_t readCount;
        while (true) {
            readCount = read(connFd, buff, nBytes);
            std::cout << "readCount: " << readCount << std::endl;
            if (readCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    return IoStatus::Ok;
                    //this->logger.infoLn("EAGAIN or EWOULDBLOCK");
                } else {
                    return IoStatus::Error;
                }
            } else if (readCount == 0) {
                return IoStatus::Disconnected;
            }
            this->inputData.insert(this->inputData.end(), buff, buff + readCount);
            std::cout << "===============================" << std::endl;
            for (int i = 0; i < this->inputData.size(); i++) {
                std::cout << "[" << this->inputData[i] << "]";
                if (i % 16 == 15) {
                    std::cout << std::endl;
                }
            }
            std::cout << "===============================" << std::endl;
            return IoStatus::Ok;
        }
    }
}