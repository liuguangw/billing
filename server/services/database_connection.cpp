//
// Created by liuguang on 2021/12/19.
//

#include "database_connection.h"

namespace services {
    bool DatabaseConnection::connect() {
        return mysql_real_connect(this->mysql, this->host, this->user,
                                             this->passwd, this->db, this->port,
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