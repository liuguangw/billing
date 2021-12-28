//
// Created by liuguang on 2021/12/23.
//

#include "handler_resource.h"

namespace services {

    void HandlerResource::initConfig(const char *configFilePath) {
        this->mServerConfig.initConfig(configFilePath);
    }

    void HandlerResource::initLogger(const char *logFilePath) {
        this->mLogger.initLogger(logFilePath);
    }

    void HandlerResource::initDatabase() {
        this->mDatabaseConnection = new DatabaseConnection(this->mServerConfig);
        this->mDatabaseConnection->connect();
    }

    void HandlerResource::initResource(const char *configFilePath, const char *logFilePath) {
        this->initConfig(configFilePath);
        this->initLogger(logFilePath);
    }

    HandlerResource::~HandlerResource() {
        delete this->mDatabaseConnection;
    }
}