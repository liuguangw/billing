//
// Created by liuguang on 2021/12/24.
//
#include <cstring>
#include "libs.h"
#include "../common/billing_exception.h"

namespace models {

    void loadAccountByUsername(DatabaseConnection *dbConn, const char *username, Account *account, bool *exists) {
        using common::BillingException;

        auto mysql = dbConn->getMysql();
        auto stmt = mysql_stmt_init(mysql);
        if (!stmt) {
            throw BillingException("mysql_stmt_init failed", dbConn->lastError());
        }
        const char *sql = "SELECT id,name,password"
                          ",question,answer,email,id_card,point"
                          " FROM account WHERE name=?";
        if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
            throw BillingException("mysql_stmt_prepare failed", dbConn->lastError());
        }
        unsigned long nameSize = strlen(username);
        MYSQL_BIND bind[1];
        my_bool isNull = 0;
        memset(bind, 0, sizeof(bind));
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void *) username;
        bind[0].buffer_length = nameSize + 1;
        bind[0].is_null = &isNull;
        bind[0].length = &nameSize;
        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            throw BillingException("mysql_stmt_bind_param failed", dbConn->lastError());
        }
        auto prepare_meta_result = mysql_stmt_result_metadata(stmt);
        if (!prepare_meta_result) {
            throw BillingException("mysql_stmt_result_metadata failed", dbConn->lastError());
        }
        if (mysql_stmt_execute(stmt) != 0) {
            throw BillingException("mysql_stmt_execute failed", dbConn->lastError());
        }
        //
        const unsigned int STR_MAX_SIZE = 60;
        char nameBuffer[STR_MAX_SIZE];
        char passwordBuffer[STR_MAX_SIZE];
        char questionBuffer[STR_MAX_SIZE];
        char answerBuffer[STR_MAX_SIZE];
        char emailBuffer[STR_MAX_SIZE];
        char idCardBuffer[STR_MAX_SIZE];
        //
        const unsigned int fieldCount = 8;
        MYSQL_BIND bindr[fieldCount];
        memset(bindr, 0, sizeof(bindr));
        unsigned long length[fieldCount];
        my_bool is_null[fieldCount];
        my_bool error[fieldCount];
        int i = 0;
        bindr[i].buffer_type = MYSQL_TYPE_LONG;
        bindr[i].buffer = &account->ID;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &nameBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &passwordBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &questionBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &answerBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &emailBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_STRING;
        bindr[i].buffer = &idCardBuffer;
        bindr[i].buffer_length = STR_MAX_SIZE;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        i++;
        bindr[i].buffer_type = MYSQL_TYPE_LONG;
        bindr[i].buffer = &account->Point;
        bindr[i].is_null = &is_null[i];
        bindr[i].length = &length[i];
        bindr[i].error = &error[i];
        //
        if (mysql_stmt_bind_result(stmt, bindr) != 0) {
            throw BillingException("mysql_stmt_bind_result failed", dbConn->lastError());
        }
        if (mysql_stmt_store_result(stmt) != 0) {
            throw BillingException("mysql_stmt_store_result failed", dbConn->lastError());
        }
        int fetchResult = mysql_stmt_fetch(stmt);
        if (fetchResult == 0) {
            account->Name = nameBuffer;
            account->Password = passwordBuffer;
            account->Answer = answerBuffer;
            account->Email = emailBuffer;
            account->IDCard = idCardBuffer;
            *exists = true;
        } else if (fetchResult == MYSQL_NO_DATA) {
            *exists = false;
        } else {
            throw BillingException("mysql_stmt_fetch failed", dbConn->lastError());
        }
        if (mysql_stmt_free_result(stmt) != 0) {
            throw BillingException("mysql_stmt_free_result failed", dbConn->lastError());
        }
        if (mysql_stmt_close(stmt) != 0) {
            throw BillingException("mysql_stmt_close failed", dbConn->lastError());
        }
    }
}