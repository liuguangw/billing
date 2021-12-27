//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_LIBS_H
#define BILLING_LIBS_H

#include <map>
#include "account.h"
#include "login_result.h"
#include "register_result.h"
#include "../services/database_connection.h"
#include "../common/client_info.h"

namespace models {
    using services::DatabaseConnection;

    /**
     * 根据用户名加载用户信息
     * @param dbConn
     * @param username 用户名
     * @param account 用于写入用户数据
     * @return 用户是否存在
     * @throws common::BillingException
     */
    bool loadAccountByUsername(DatabaseConnection *dbConn, const char *username, Account &account);

    /**
     * 注册帐号
     * @param registerResult
     * @param dbConn
     * @param account 用户信息
     * @return 注册结果
     */
    RegisterResult registerAccount(DatabaseConnection *dbConn, const Account &account);

    /**
     * 插入用户记录
     * @param dbConn
     * @param account
     * @throws common::BillingException
     */
    void doInsertAccount(DatabaseConnection *dbConn, const Account &account);

    /**
     * 点数兑换
     * @param dbConn
     * @param username
     * @param point
     * @throws common::BillingException
     */
    void convertUserPoint(DatabaseConnection *dbConn, const char *username, unsigned int point);

    /**
     * 登录验证
     * @param loginResult
     * @param dbConn
     * @param username
     * @param password
     */
    LoginResult checkLogin(DatabaseConnection *dbConn, std::map<string, common::ClientInfo> &onlineUsers,
                           const char *username, const char *password);
}
#endif //BILLING_LIBS_H
