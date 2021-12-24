//
// Created by liuguang on 2021/12/24.
//
#include "libs.h"
#include "../common/billing_exception.h"

namespace models {
    void checkLogin(LoginResult *loginResult, DatabaseConnection *dbConn,
                    std::map<std::string, common::ClientInfo> *onlineUsers, const char *username,
                    const char *password) {
        using common::BillingException;
        Account account{};
        bool exists = false;
        try {
            loadAccountByUsername(dbConn, username, &account, &exists);
        } catch (BillingException &ex) {
            loginResult->loginError = LoginError::LoginOtherError;
            loginResult->message = ex.what();
            return;
        }
        if (!exists) {
            loginResult->loginError = LoginError::LoginUserNotFound;
            loginResult->message = "login user not found";
            return;
        }
        if (account.Password != password) {
            loginResult->loginError = LoginError::LoginInvalidPassword;
            loginResult->message = "invalid password";
            return;
        }
        if (account.IDCard == "1") {
            loginResult->loginError = LoginError::LoginAccountLocked;
            loginResult->message = "account locked";
            return;
        }
        //判断用户是否在线
        auto it = onlineUsers->find(username);
        if (it != onlineUsers->end()) {
            loginResult->loginError = LoginError::LoginAccountOnline;
            loginResult->message = "account role is online";
            return;
        }
        loginResult->loginError = LoginError::LoginNoError;
        loginResult->message = "success";
    }
}