//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_SERVER_H
#define BILLING_SERVER_H

#include <sys/epoll.h>
#include <netinet/in.h>
#include <map>
#include "../common/server_config.h"
#include "database_connection.h"
#include "logger.h"
#include "tcp_connection.h"

namespace services {
    class Server {
    private:
        common::ServerConfig serverConfig;
        Logger logger;
        DatabaseConnection *databaseConnection = nullptr;
        int signalFd = -1, serverSockFd = -1, epollFd = -1;
        std::map<int, TcpConnection *> tcpConnections;

        void initDatabase();

        //初始化epoll
        void initEpoll();

        //初始化信号fd
        void initSignal();

        //初始化tcp监听
        void initListener();

        void runLoop();

        //接受tcp连接
        void processAcceptConnEvent(epoll_event *connEvent);

        //处理tcp连接的事件
        void processConnEvent(epoll_event *connEvent);

    public:
        ~Server();

        void initConfig(const char *configFilePath);

        void initLogger(const char *logFilePath);

        int run();
    };
}


#endif //BILLING_SERVER_H
