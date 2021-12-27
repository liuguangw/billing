//
// Created by liuguang on 2021/12/24.
//
#include "libs.h"
#include "../common/billing_exception.h"

namespace models {
    LoginResult checkLogin(DatabaseConnection *dbConn, std::map<string, common::ClientInfo> &onlineUsers,
                           const char *username, const char *password) {
        using common::BillingException;
        LoginResult loginResult{
                .loginError=LoginError::LoginNoError,
                .message="success"
        };
        Account account{};
        bool exists = false;
        try {
            exists = loadAccountByUsername(dbConn, username, account);
        } catch (BillingException &ex) {
            loginResult.loginError = LoginError::LoginOtherError;
            loginResult.message = ex.what();
            return loginResult;
        }
        if (!exists) {
            loginResult.loginError = LoginError::LoginUserNotFound;
            loginResult.message = "login user not found";
            return loginResult;
        }
        if (account.Password != password) {
            loginResult.loginError = LoginError::LoginInvalidPassword;
            loginResult.message = "invalid password";
            return loginResult;
        }
        if (account.IDCard == "1") {
            loginResult.loginError = LoginError::LoginAccountLocked;
            loginResult.message = "account locked";
            return loginResult;
        }
        //判断用户是否在线
        auto it = onlineUsers.find(username);
        if (it != onlineUsers.end()) {
            loginResult.loginError = LoginError::LoginAccountOnline;
            loginResult.message = "account role is online";
            return loginResult;
        }
        return loginResult;
    }
}