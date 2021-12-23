//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_HANDLER_RESOURCE_H
#define BILLING_HANDLER_RESOURCE_H

#include "../common/server_config.h"
#include "database_connection.h"
#include "logger.h"

namespace services {
    class HandlerResource {
    private:
        common::ServerConfig mServerConfig;
        Logger mLogger;
        DatabaseConnection *mDatabaseConnection = nullptr;

        void initConfig(const char *configFilePath);

        void initLogger(const char *logFilePath);

    public:
        common::ServerConfig *config() {
            return &this->mServerConfig;
        }

        Logger *logger() {
            return &this->mLogger;
        }

        DatabaseConnection *Conn() {
            return this->mDatabaseConnection;
        }

        void initResource(const char *configFilePath, const char *logFilePath);

        void initDatabase();
        ~HandlerResource();
    };
}

#endif //BILLING_HANDLER_RESOURCE_H
