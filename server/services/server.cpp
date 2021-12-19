//
// Created by liuguang on 2021/12/19.
//

#include "server.h"
#include "load_server_config.h"
#include <iostream>
#include <vector>

namespace services {
    Server::Server() {
        using std::string;
        using std::vector;
        this->serverConfig = new common::ServerConfig;
        this->databaseConnection = NULL;
    }

    Server::~Server() {
        delete this->serverConfig;
        if (this->databaseConnection != NULL) {
            delete this->databaseConnection;
        }
    }

    void Server::initConfig(const char *configFilePath) {
        loadServerConfig(configFilePath, this->serverConfig);
        std::cout << "listen addr: " << this->serverConfig->IP << ":" << this->serverConfig->Port << std::endl;
        std::cout << "mysql: " << this->serverConfig->DbHost << ":" << this->serverConfig->DbPort << std::endl;
    }

    void Server::initDatabase() {
        this->databaseConnection = new DatabaseConnection(this->serverConfig);
        if (!this->databaseConnection->connect()) {
            throw this->databaseConnection->lastError();
        }
    }
}
