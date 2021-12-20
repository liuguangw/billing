//
// Created by liuguang on 2021/12/19.
//

#include "server.h"
#include "load_server_config.h"
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>


namespace services {
    Server::Server() {
        this->databaseConnection = NULL;
        this->serverSockFd = -1;
        this->epollFd = -1;
    }

    Server::~Server() {
        if (this->databaseConnection != NULL) {
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
        if (!this->databaseConnection->connect()) {
            throw this->databaseConnection->lastError();
        }
    }

    void Server::initListener() {
        this->serverSockFd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (this->serverSockFd < 0) {
            throw errno;
        }
        this->serverAddress.sin_family = AF_INET;
        this->serverAddress.sin_addr.s_addr = inet_addr(this->serverConfig.IP.c_str());
        this->serverAddress.sin_port = htons(this->serverConfig.Port);
        if (bind(this->serverSockFd, (sockaddr *) &this->serverAddress, sizeof(this->serverAddress)) < 0) {
            throw errno;
        }
        if (listen(this->serverSockFd, 10) < 0) {
            throw errno;
        }
        //初始化epoll
        this->epollFd = epoll_create1(0);
        if (this->epollFd == -1) {
            throw errno;
        }
        //注册事件
        this->serverEpollEvent.events = EPOLLIN;
        this->serverEpollEvent.data.fd = this->serverSockFd;
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, this->serverSockFd, &this->serverEpollEvent) == -1) {
            throw errno;
        }
    }

    int Server::run() {
        using std::string;
        /*try {
            this->initDatabase();
        } catch (const char *connErr) {
            string errorInfo = "connect database failed: ";
            errorInfo.append(connErr);
            this->logger.errorLn(errorInfo.c_str());
            return EXIT_FAILURE;
        }
        string serverVersionInfo = "mysql version: ";
        serverVersionInfo.append(this->databaseConnection->serverVersion());
        this->logger.infoLn(serverVersionInfo.c_str());*/
        try {
            this->initListener();
        } catch (int errCode) {
            this->logger.errorLn(strerror(errCode));
            return EXIT_FAILURE;
        }
        string listenAddr = this->serverConfig.IP;
        listenAddr += ":";
        char buffer [8];
        sprintf(buffer,"%d",this->serverConfig.Port);
        listenAddr += buffer;
        this->logger.infoLn(listenAddr.c_str());
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
        int nfds, conn_sock;
        char buff[4096];
        for (;;) {
            nfds = epoll_wait(this->epollFd, events, MAX_EVENTS, 5000);
            if (nfds == -1) {
                std::cout<<"aaa"<<std::endl;
                throw errno;
            }

            for (int n = 0; n < nfds; ++n) {
                if (events[n].data.fd == this->serverSockFd) {
                    conn_sock = accept4(this->serverSockFd,
                                        (struct sockaddr *) NULL, NULL, SOCK_NONBLOCK);
                    if (conn_sock == -1) {
                        std::cout<<"bbb"<<std::endl;
                        throw errno;
                    }
                    this->serverEpollEvent.events = EPOLLIN | EPOLLET;
                    this->serverEpollEvent.data.fd = conn_sock;
                    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, conn_sock,
                                  &this->serverEpollEvent) == -1) {
                        std::cout<<"ccc"<<std::endl;
                        throw errno;
                    }
                } else {
                    int connFd = events[n].data.fd;
                    auto p = recv(connFd, buff, 4096, 0);
                    buff[p] = '\0';
                    this->logger.infoLn(buff);
                    close(connFd);
                }
            }
        }
        return EXIT_SUCCESS;
    }
}
