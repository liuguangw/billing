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
        DatabaseConnection *databaseConnection;
        int serverSockFd, epollFd;
        std::map<int,TcpConnection*> tcpConnections;

        void initDatabase();

        void initListener();

        void processConnEvent(epoll_event* connEvent,unsigned char *buff, size_t nBytes);
        void freeAllConnections();

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
