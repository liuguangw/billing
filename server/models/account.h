//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_ACCOUNT_H
#define BILLING_ACCOUNT_H

#include <string>

namespace models {
    using std::string;

    struct Account {
        int ID;        //账号id
        string Name,        //用户名
        Password,        //已加密的密码
        Question, //密保问题(现在存放的是加密的超级密码)
        Answer, //密保答案
        Email, //注册时填写的邮箱
        IDCard; //用于判定账号是否锁定,如果是字符串1就说明账号已锁定
        int Point;            //点数
    };
}
#endif //BILLING_ACCOUNT_H
