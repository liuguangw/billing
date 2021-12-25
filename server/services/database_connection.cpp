//
// Created by liuguang on 2021/12/19.
//

#include "database_connection.h"
#include "../common/billing_exception.h"

namespace services {
    void DatabaseConnection::connect() {
        mysql_options(this->mysql, MYSQL_OPT_RECONNECT, &this->reConnect);
        if (mysql_real_connect(this->mysql, this->serverConfig->DbHost.c_str(), this->serverConfig->DbUser.c_str(),
                               this->serverConfig->DbPassword.c_str(), this->serverConfig->DbName.c_str(),
                               this->serverConfig->DbPort,
                               nullptr, 0) == nullptr) {
            throw common::BillingException("connect database failed", this->lastError());
        }
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

    MYSQL *DatabaseConnection::getMysql() {
        return this->mysql;
    }
}
