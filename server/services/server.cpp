//
// Created by liuguang on 2021/12/19.
//

#include "server.h"
#include "load_server_config.h"
#include "../common/billing_exception.h"
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <sstream>


namespace services {
    Server::Server() {
        this->serverSockFd = -1;
        this->epollFd = -1;
    }

    Server::~Server() {
        if (this->databaseConnection != nullptr) {
            delete this->databaseConnection;
        }
        if (this->epollFd != -1) {
            close(this->epollFd);
        }
        if (this->serverSockFd != -1) {
            close(this->serverSockFd);
        }
    }

    void Server::initConfig(const char *configFilePath) {
        loadServerConfig(configFilePath, &this->serverConfig);
    }

    void Server::initLogger(const char *logFilePath) {
        this->logger.initLogger(logFilePath);
    }

    void Server::initDatabase() {
        this->databaseConnection = new DatabaseConnection(&this->serverConfig);
        this->databaseConnection->connect();
    }

    void Server::initListener() {
        this->serverSockFd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (this->serverSockFd < 0) {
            throw common::BillingException("init server socket failed",errno);
        }
        sockaddr_in  serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(this->serverConfig.IP.c_str());
        serverAddress.sin_port = htons(this->serverConfig.Port);
        if (bind(this->serverSockFd, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            throw common::BillingException("server socket bind failed",errno);
        }
        if (listen(this->serverSockFd, 10) < 0) {
            throw common::BillingException("server socket listen failed",errno);
        }
        //初始化epoll
        this->epollFd = epoll_create1(0);
        if (this->epollFd == -1) {
            throw common::BillingException("init epoll fd failed",errno);
        }
        //注册事件
        epoll_event epollEvent{};
        epollEvent.events = EPOLLIN;
        epollEvent.data.fd = this->serverSockFd;
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, this->serverSockFd, &epollEvent) == -1) {
            throw common::BillingException("epoll register server socket failed",errno);
        }
    }

    int Server::run() {
        using std::string;
        try {
            this->initDatabase();
        } catch (common::BillingException& ex) {
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        string serverVersionInfo = "mysql version: ";
        serverVersionInfo.append(this->databaseConnection->serverVersion());
        this->logger.infoLn(serverVersionInfo.c_str());
        //
        try {
            this->initListener();
        } catch (common::BillingException& ex) {
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        {
            std::stringstream listenAddress;
            listenAddress<< this->serverConfig.IP.c_str()<<":"<<this->serverConfig.Port;
            this->logger.infoLn(listenAddress.str().c_str());
        }
        try {
            this->runLoop();
        } catch (int errCode) {
            this->logger.errorLn(strerror(errCode));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    int Server::runLoop() {
        const int MAX_EVENTS = 10;
        epoll_event events[MAX_EVENTS];
        epoll_event epollEvent{};
        int fdCount, conn_sock;
        char buff[4096];
        for (;;) {
            fdCount = epoll_wait(this->epollFd, events, MAX_EVENTS, 5000);
            if (fdCount == -1) {
                throw common::BillingException("epoll wait failed",errno);
            }

            for (int n = 0; n < fdCount; ++n) {
                if (events[n].data.fd == this->serverSockFd) {
                    conn_sock = accept4(this->serverSockFd,
                                        (struct sockaddr *) nullptr, nullptr, SOCK_NONBLOCK);
                    if (conn_sock == -1) {
                        throw common::BillingException("accept connection failed",errno);
                    }
                    this->logger.infoLn("connected");
                    epollEvent.events = EPOLLIN | EPOLLET;
                    epollEvent.data.fd = conn_sock;
                    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, conn_sock,
                                  &epollEvent) == -1) {
                        throw common::BillingException("epoll register connection socket failed",errno);
                    }
                } else {
                    int connFd = events[n].data.fd;
                    ssize_t readCount;
                    while (true) {
                        readCount = read(connFd, buff, 4096);
                        std::cout << "readCount: "<<readCount << std::endl;
                        if (readCount < 0) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                //this->logger.infoLn("EAGAIN or EWOULDBLOCK");
                            } else {
                                this->logger.errorLn("read error");
                            }
                            break;
                        }
                        if (readCount == 0) {
                            this->logger.infoLn("disconnected");
                            close(connFd);
                            break;
                        }
                        buff[readCount] = '\0';
                        this->logger.infoLn(buff);
                    }
                }
            }
        }
        return EXIT_SUCCESS;
    }
}
