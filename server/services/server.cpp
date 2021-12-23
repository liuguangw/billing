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
#include <sys/signalfd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <csignal>


namespace services {
    Server::~Server() {
        delete this->databaseConnection;
        if (this->epollFd != -1) {
            close(this->epollFd);
        }
        if (this->signalFd != -1) {
            close(this->signalFd);
        }
        if (this->serverSockFd != -1) {
            close(this->serverSockFd);
        }
        for (auto &tcpConnection: this->tcpConnections) {
            delete tcpConnection.second;
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

    void Server::initEpoll() {
        this->epollFd = epoll_create1(0);
        if (this->epollFd == -1) {
            throw common::BillingException("init epoll fd failed", errno);
        }
    }

    void Server::initSignal() {
        //阻塞信号
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        sigaddset(&mask, SIGQUIT);
        sigaddset(&mask, SIGTERM);
        if (sigprocmask(SIG_BLOCK, &mask, nullptr) != 0) {
            throw common::BillingException("block signal failed", errno);
        }
        //fd关注的信号类型
        this->signalFd = signalfd(-1, &mask, SFD_NONBLOCK);
        if (this->signalFd < 0) {
            throw common::BillingException("init signal failed", errno);
        }
        //注册事件
        epoll_event epollEvent{};
        epollEvent.events = EPOLLIN;
        epollEvent.data.fd = this->signalFd;
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, this->signalFd, &epollEvent) == -1) {
            throw common::BillingException("epoll register signal failed", errno);
        }
    }

    void Server::initListener() {
        this->serverSockFd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (this->serverSockFd < 0) {
            throw common::BillingException("init server socket failed", errno);
        }
        int option = 1;
        setsockopt(this->serverSockFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
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
            this->initEpoll();
            this->initSignal();
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
            this->logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        this->logger.infoLn("server stopped");
        return EXIT_SUCCESS;
    }

    void Server::runLoop() {
        const int MAX_EVENTS = 20;
        epoll_event events[MAX_EVENTS];
        int fdCount;
        while (true) {
            fdCount = epoll_wait(this->epollFd, events, MAX_EVENTS, -1);
            if (fdCount < 0) {
                throw common::BillingException("epoll wait failed", errno);
            }
            for (int n = 0; n < fdCount; ++n) {
                int fd = events[n].data.fd;
                if (fd == this->signalFd) {
                    //收到停止信号了
                    this->logger.infoLn("get stop signal");
                    return;
                } else if (fd == this->serverSockFd) {
                    this->processAcceptConnEvent(&events[n]);
                } else {
                    this->processConnEvent(&events[n]);
                }
            }
        }
    }

    void Server::processAcceptConnEvent(epoll_event *connEvent) {
        int connFd = accept4(this->serverSockFd,
                             (struct sockaddr *) nullptr, nullptr, SOCK_NONBLOCK);
        if (connFd < 0) {
            std::stringstream msg;
            msg << "accept connection failed: " << strerror(errno);
            this->logger.errorLn(msg.str().c_str());
            return;
        }
        //为tcp连接注册事件
        connEvent->events = EPOLLIN | EPOLLOUT | EPOLLET;
        connEvent->data.fd = connFd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, connEvent) == -1) {
            std::stringstream msg;
            msg << "epoll register connection socket failed: " << strerror(errno);
            this->logger.errorLn(msg.str().c_str());
            return;
        }
        //分配空间,放入map中
        auto tcpConn = new TcpConnection(connFd, &this->logger);
        this->tcpConnections[connFd] = tcpConn;
        this->logger.infoLn("accept ok");
    }

    void Server::processConnEvent(epoll_event *connEvent) {
        auto tcpConn = this->tcpConnections[connEvent->data.fd];
        if (!tcpConn->processConn(
                (connEvent->events & EPOLLIN) != 0,
                (connEvent->events & EPOLLOUT) != 0) ){
            //释放空间
            this->tcpConnections.erase(connEvent->data.fd);
            delete tcpConn;
        }
    }
}
