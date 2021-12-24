//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_LOGIN_RESULT_H
#define BILLING_LOGIN_RESULT_H
#include <string>

namespace models{
    //登录错误定义
    enum LoginError{
        //没有错误
        LoginNoError,
        //登录的用户不存在
        LoginUserNotFound,
        //密码错误
        LoginInvalidPassword,
        // 账号停权
        LoginAccountLocked,
        // 有角色在线
        LoginAccountOnline,
        // 其他错误
        LoginOtherError
    };

    struct LoginResult{
        LoginError loginError;
        std::string message;
    };
}

#endif //BILLING_LOGIN_RESULT_H
