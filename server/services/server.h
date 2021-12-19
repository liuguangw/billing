//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_SERVER_H
#define BILLING_SERVER_H

#include "../common/server_config.h"
#include "database_connection.h"

namespace services {
    class Server {
    private:
        common::ServerConfig *serverConfig;
        DatabaseConnection* databaseConnection;
    public:
        Server();

        ~Server();

        void initConfig(const char *configFilePath);
        void initDatabase();
    };
}


#endif //BILLING_SERVER_H
