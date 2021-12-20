//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_SERVER_H
#define BILLING_SERVER_H

#include <sys/epoll.h>
#include <netinet/in.h>
#include "../common/server_config.h"
#include "database_connection.h"
#include "logger.h"

namespace services {
    class Server {
    private:
        common::ServerConfig serverConfig;
        Logger logger;
        DatabaseConnection *databaseConnection;
        sockaddr_in  serverAddress;
        int serverSockFd,epollFd;
        epoll_event serverEpollEvent;

        void initDatabase();

        void initListener();

    public:
        Server();

        ~Server();

        void initConfig(const char *configFilePath);

        void initLogger(const char *logFilePath);

        int run();
        int runLoop();
    };
}


#endif //BILLING_SERVER_H
