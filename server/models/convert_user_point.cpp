//
// Created by liuguang on 2021/12/25.
//

#include <cstring>
#include "libs.h"
#include "../common/billing_exception.h"

namespace models {
    void convertUserPoint(DatabaseConnection *dbConn, const char *username, unsigned int point) {
        using common::BillingException;

        auto mysql = dbConn->getMysql();
        auto stmt = mysql_stmt_init(mysql);
        if (!stmt) {
            throw BillingException("mysql_stmt_init failed", dbConn->lastError());
        }
        const char *sql = "UPDATE account SET point=point-? WHERE name=?";
        if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
            throw BillingException("mysql_stmt_prepare failed", dbConn->lastError());
        }
        MYSQL_BIND bind[2];
        memset(bind, 0, sizeof(bind));
        unsigned char offset = 0;
        bind[offset].buffer_type = MYSQL_TYPE_LONG;
        bind[offset].buffer = &point;
        bind[offset].is_null = nullptr;
        bind[offset].length = nullptr;
        offset++;
        //
        unsigned long nameSize = strlen(username);
        bind[offset].buffer_type = MYSQL_TYPE_STRING;
        bind[offset].buffer = (void *) username;
        bind[offset].buffer_length = nameSize + 1;
        bind[offset].is_null = nullptr;
        bind[offset].length = &nameSize;
        //
        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            throw BillingException("mysql_stmt_bind_param failed", dbConn->lastError());
        }
        if (mysql_stmt_execute(stmt) != 0) {
            throw BillingException("mysql_stmt_execute failed", dbConn->lastError());
        }
        if (mysql_stmt_close(stmt) != 0) {
            throw BillingException("mysql_stmt_close failed", dbConn->lastError());
        }
    }
}