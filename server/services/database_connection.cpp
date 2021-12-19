//
// Created by liuguang on 2021/12/19.
//

#include "database_connection.h"

namespace services {
    bool DatabaseConnection::connect() {
        mysql_options(this->mysql,MYSQL_OPT_RECONNECT,&this->reConnect);
        return mysql_real_connect(this->mysql, this->serverConfig->DbHost.c_str(), this->serverConfig->DbUser.c_str(),
                                             this->serverConfig->DbPassword.c_str(), this->serverConfig->DbName.c_str(),
                                             this->serverConfig->DbPort,
                                             NULL, 0) != NULL;
    }

    const char *DatabaseConnection::lastError() {
        return mysql_error(this->mysql);
    }

    DatabaseConnection::~DatabaseConnection() {
        mysql_close(this->mysql);
    }

    const char *DatabaseConnection::serverVersion() {
        return mysql_get_server_info(this->mysql);
    }
}