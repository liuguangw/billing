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
#include <csignal>


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
            throw common::BillingException("init server socket failed", errno);
        }
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(this->serverConfig.IP.c_str());
        serverAddress.sin_port = htons(this->serverConfig.Port);
        if (bind(this->serverSockFd, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            throw common::BillingException("server socket bind failed", errno);
        }
        if (listen(this->serverSockFd, 10) < 0) {
            throw common::BillingException("server socket listen failed", errno);
        }
        //初始化epoll
        this->epollFd = epoll_create1(0);
        if (this->epollFd == -1) {
            throw common::BillingException("init epoll fd failed", errno);
        }
        //注册事件
        epoll_event epollEvent{};
        epollEvent.events = EPOLLIN;
        epollEvent.data.fd = this->serverSockFd;
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, this->serverSockFd, &epollEvent) == -1) {
            throw common::BillingException("epoll register server socket failed", errno);
        }
    }

    int Server::run() {
        /*try {
            this->initDatabase();
        } catch (common::BillingException &ex) {
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        string serverVersionInfo = "mysql version: ";
        serverVersionInfo.append(this->databaseConnection->serverVersion());
        this->logger.infoLn(serverVersionInfo.c_str());*/
        //
        try {
            this->initListener();
        } catch (common::BillingException &ex) {
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        {
            std::stringstream listenAddress;
            listenAddress << this->serverConfig.IP.c_str() << ":" << this->serverConfig.Port;
            this->logger.infoLn(listenAddress.str().c_str());
        }
        try {
            this->runLoop();
        } catch (common::BillingException &ex) {
            this->freeAllConnections();
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        this->freeAllConnections();
        this->logger.infoLn("server stopped");
        return EXIT_SUCCESS;
    }

    int Server::runLoop() {
        const int MAX_EVENTS = 20;
        epoll_event events[MAX_EVENTS];
        epoll_event epollEvent{};
        int fdCount;
        const size_t buffSize = 4096;
        unsigned char buff[buffSize];
        //signal
        sigset_t stopSignal{},outputSignal{};
        sigemptyset(&stopSignal);
        sigaddset(&stopSignal,SIGINT);
        sigaddset(&stopSignal,SIGTERM);
        sigprocmask(SIG_BLOCK,&stopSignal,&outputSignal);
        while (true) {
            fdCount = epoll_pwait(this->epollFd, events, MAX_EVENTS, 5000,&stopSignal);
            if (fdCount < 0) {
                if(errno == EINTR){
                    this->logger.infoLn("get signal");
                    break;
                }
                throw common::BillingException("epoll wait failed", errno);
            }
            this->logger.infoLn("get wait result");

            for (int n = 0; n < fdCount; ++n) {
                int connFd = events[n].data.fd;
                if (connFd == this->serverSockFd) {
                    auto tcpConn = new TcpConnection;
                    try {
                        tcpConn->initConnection(this->epollFd, connFd, &epollEvent);
                    } catch (common::BillingException &ex) {
                        delete tcpConn;
                        this->logger.errorLn(ex.what());
                        continue;
                    }
                    this->tcpConnections[tcpConn->getConnFd()] = tcpConn;
                } else {
                    this->processConnEvent(&events[n], buff, buffSize);
                }
            }
        }
        return EXIT_SUCCESS;
    }

    void Server::processConnEvent(epoll_event *connEvent, unsigned char *buff, size_t nBytes) {
        using common::IoStatus;
        int connFd = connEvent->data.fd;
        auto tcpConn = this->tcpConnections[connFd];
        if ((connEvent->events & EPOLLIN) != 0) {
            auto ioStatus = tcpConn->readAll(buff, nBytes);
            if (ioStatus != IoStatus::Ok) {
                std::stringstream ss;
                if (ioStatus == IoStatus::Error) {
                    ss << "read client(fd:" << connFd << ") error," << strerror(errno);
                    this->logger.infoLn(ss.str().c_str());
                } else if (ioStatus == IoStatus::Disconnected) {
                    ss << "read client(fd:" << connFd << ") disconnected";
                    this->logger.infoLn(ss.str().c_str());
                }
                this->tcpConnections.erase(connFd);
                delete tcpConn;
                return;
            }
        }
        if ((connEvent->events & EPOLLOUT) != 0) {
            tcpConn->setWriteAble(true);
        }
    }

    void Server::freeAllConnections() {
        for(auto it=this->tcpConnections.begin();it!= this->tcpConnections.end();it++){
            delete it->second;
        }
    }
}
