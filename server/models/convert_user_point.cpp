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
        const unsigned int fieldCount = 2;
        MYSQL_BIND bindArr[fieldCount];
        my_bool isNullArr[fieldCount];
        memset(bindArr, 0, sizeof(bindArr));
        memset(isNullArr, 0, sizeof(isNullArr));
        unsigned char offset = 0;
        unsigned long nameSize = strlen(username);
        bindArr[offset].buffer_type = MYSQL_TYPE_STRING;
        bindArr[offset].buffer = (void *) username;
        bindArr[offset].buffer_length = nameSize + 1;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &nameSize;
        offset++;
        unsigned long pointLength = sizeof(point);
        bindArr[offset].buffer_type = MYSQL_TYPE_LONG;
        bindArr[offset].buffer = &point;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &pointLength;
        //
        if (mysql_stmt_bind_param(stmt, bindArr) != 0) {
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