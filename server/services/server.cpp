//
// Created by liuguang on 2021/12/19.
//

#include <string>
#include <cstring>
#include <csignal>
#include <sstream>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include "server.h"
#include "../common/billing_exception.h"


namespace services {
    Server::~Server() {
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
        auto serverConfig = this->handlerResource.config();
        serverAddress.sin_addr.s_addr = inet_addr(serverConfig->IP.c_str());
        serverAddress.sin_port = htons(serverConfig->Port);
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
        auto logger = this->handlerResource.logger();
        try {
            this->handlerResource.initDatabase();
        } catch (common::BillingException &ex) {
            logger->errorLn(ex.what());
            return EXIT_FAILURE;
        }
        std::stringstream ss;
        ss << "mysql version: " << this->handlerResource.DbConn()->serverVersion();
        logger->infoLn(&ss);
        //
        try {
            this->initEpoll();
            this->initSignal();
            this->initListener();
        } catch (common::BillingException &ex) {
            logger->errorLn(ex.what());
            return EXIT_FAILURE;
        }
        {
            auto serverConfig = this->handlerResource.config();
            ss.str("");
            ss << "billing server run at " << serverConfig->IP.c_str() << ":" << serverConfig->Port;
            logger->infoLn(&ss);
        }
        try {
            this->runLoop();
        } catch (common::BillingException &ex) {
            logger->errorLn(ex.what());
            return EXIT_FAILURE;
        }
        logger->infoLn("server stopped");
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
                    this->handlerResource.logger()->infoLn("get stop signal");
                    return;
                } else if (fd == this->serverSockFd) {
                    this->processAcceptConnEvent(&events[n]);
                } else {
                    this->processConnEvent(&events[n]);
                }
            }
        }
    }

    bool Server::allowIpAddress(const char *address) {
        auto allowIps = &this->handlerResource.config()->AllowIps;
        // 当配置的白名单为空时,表示允许所有ip连接
        if (allowIps->empty()) {
            return true;
        }
        // 当不为空时,只允许指定的ip连接
        bool ipAllowed = false;
        for (auto &allowIp: *allowIps) {
            if (allowIp == address) {
                ipAllowed = true;
            }
        }
        return ipAllowed;
    }

    void Server::processAcceptConnEvent(epoll_event *connEvent) {
        auto logger = this->handlerResource.logger();
        sockaddr_in remoteAddress{};
        socklen_t remoteAddressLength = sizeof(remoteAddress);
        int connFd = accept4(this->serverSockFd,
                             (struct sockaddr *) &remoteAddress, &remoteAddressLength, SOCK_NONBLOCK);
        if (connFd < 0) {
            std::stringstream ss;
            ss << "accept connection failed: " << strerror(errno);
            logger->errorLn(&ss);
            return;
        }
        //获取连接者的ip和端口
        char *ipAddressStr = inet_ntoa(remoteAddress.sin_addr);
        unsigned short port = ntohs(remoteAddress.sin_port);
        if (!this->allowIpAddress(ipAddressStr)) {
            close(connFd);
            std::stringstream ss;
            ss << "client " << ipAddressStr << " is not allowed to connect";
            logger->errorLn(&ss);
            return;
        }
        //为tcp连接注册事件
        connEvent->events = EPOLLIN | EPOLLOUT | EPOLLET;
        connEvent->data.fd = connFd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, connEvent) == -1) {
            std::stringstream ss;
            ss << "epoll register connection socket failed: " << strerror(errno);
            logger->errorLn(&ss);
            return;
        }
        //分配空间,放入map中
        auto tcpConn = new TcpConnection(connFd, ipAddressStr, port, &this->handlerResource);
        this->tcpConnections[connFd] = tcpConn;
    }

    void Server::processConnEvent(epoll_event *connEvent) {
        auto tcpConn = this->tcpConnections[connEvent->data.fd];
        if (!tcpConn->processConn(
                (connEvent->events & EPOLLIN) != 0,
                (connEvent->events & EPOLLOUT) != 0)) {
            //释放空间
            this->tcpConnections.erase(connEvent->data.fd);
            delete tcpConn;
        }
    }

    void Server::initResource(const char *configFilePath, const char *logFilePath) {
        this->handlerResource.initResource(configFilePath, logFilePath);
    }
}
