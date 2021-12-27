//
// Created by liuguang on 2021/12/25.
//

#include <cstring>
#include <sstream>
#include "libs.h"
#include "../common/billing_exception.h"

namespace models {
    RegisterResult registerAccount(DatabaseConnection *dbConn, const Account &account) {
        using common::BillingException;
        RegisterResult registerResult{
                .hasError=false,
                .message="success"
        };
        Account tmpAccount{};
        bool exists = false;
        try {
            exists = loadAccountByUsername(dbConn, account.Name.c_str(), tmpAccount);
        } catch (BillingException &ex) {
            registerResult.hasError = true;
            registerResult.message = ex.what();
            return registerResult;
        }
        // 要注册的用户已存在
        if (exists) {
            registerResult.hasError = true;
            std::stringstream ss;
            ss << "user " << account.Name << " already exists";
            registerResult.message = ss.str();
            return registerResult;
        }
        // 邮箱不能为空
        if (account.Email.empty()) {
            registerResult.hasError = true;
            registerResult.message = "email can't be empty";
            return registerResult;
        }
        //不允许默认的邮箱
        if (account.Email == "1@1.com") {
            registerResult.hasError = true;
            registerResult.message = "email 1@1.com is not allowed";
            return registerResult;
        }
        //插入
        try {
            doInsertAccount(dbConn, account);
        } catch (BillingException &ex) {
            registerResult.hasError = true;
            registerResult.message = ex.what();
            return registerResult;
        }
        return registerResult;
    }

    void doInsertAccount(DatabaseConnection *dbConn, const Account &account) {
        using common::BillingException;

        auto mysql = dbConn->getMysql();
        auto stmt = mysql_stmt_init(mysql);
        if (!stmt) {
            throw BillingException("mysql_stmt_init failed", dbConn->lastError());
        }
        const char *sql = "INSERT INTO account (name, password, question, email) VALUES (?, ?, ?, ?)";
        if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
            throw BillingException("mysql_stmt_prepare failed", dbConn->lastError());
        }
        const unsigned int fieldCount = 4;
        MYSQL_BIND bindArr[fieldCount];
        my_bool isNullArr[fieldCount];
        unsigned long lengthArr[fieldCount];
        memset(bindArr, 0, sizeof(bindArr));
        memset(isNullArr, 0, sizeof(isNullArr));
        memset(lengthArr, 0, sizeof(lengthArr));
        unsigned char offset = 0;
        lengthArr[offset] = account.Name.length();
        bindArr[offset].buffer_type = MYSQL_TYPE_STRING;
        bindArr[offset].buffer = (void *) account.Name.c_str();
        bindArr[offset].buffer_length = lengthArr[offset] + 1;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &lengthArr[offset];
        offset++;
        lengthArr[offset] = account.Password.length();
        bindArr[offset].buffer_type = MYSQL_TYPE_STRING;
        bindArr[offset].buffer = (void *) account.Password.c_str();
        bindArr[offset].buffer_length = lengthArr[offset] + 1;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &lengthArr[offset];
        offset++;
        lengthArr[offset] = account.Question.length();
        bindArr[offset].buffer_type = MYSQL_TYPE_STRING;
        bindArr[offset].buffer = (void *) account.Question.c_str();
        bindArr[offset].buffer_length = lengthArr[offset] + 1;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &lengthArr[offset];
        offset++;
        lengthArr[offset] = account.Email.length();
        bindArr[offset].buffer_type = MYSQL_TYPE_STRING;
        bindArr[offset].buffer = (void *) account.Email.c_str();
        bindArr[offset].buffer_length = lengthArr[offset] + 1;
        bindArr[offset].is_null = &isNullArr[offset];
        bindArr[offset].length = &lengthArr[offset];
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