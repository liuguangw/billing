//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_HANDLER_RESOURCE_H
#define BILLING_HANDLER_RESOURCE_H

#include <map>
#include "../common/server_config.h"
#include "../common/client_info.h"
#include "database_connection.h"
#include "logger.h"

namespace services {
    class HandlerResource {

    public:
        const common::ServerConfig &config() {
            return this->mServerConfig;
        }

        Logger &logger() {
            return this->mLogger;
        }

        DatabaseConnection *DbConn() {
            return this->mDatabaseConnection;
        }

        std::map<std::string, common::ClientInfo> &loginUsers() {
            return this->mLoginUsers;
        }

        std::map<std::string, common::ClientInfo> &onlineUsers() {
            return this->mOnlineUsers;
        }

        std::map<std::string, unsigned int> &macCounters() {
            return this->mMacCounters;
        }

        void initResource(const char *configFilePath, const char *logFilePath);

        void initDatabase();

        ~HandlerResource();

    private:
        common::ServerConfig mServerConfig;
        Logger mLogger;
        DatabaseConnection *mDatabaseConnection = nullptr;
        std::map<std::string, common::ClientInfo> mLoginUsers;
        std::map<std::string, common::ClientInfo> mOnlineUsers;
        std::map<std::string, unsigned int> mMacCounters;

        void initConfig(const char *configFilePath);

        void initLogger(const char *logFilePath);
    };
}

#endif //BILLING_HANDLER_RESOURCE_H
