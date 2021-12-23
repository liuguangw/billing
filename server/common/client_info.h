//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_CLIENT_INFO_H
#define BILLING_CLIENT_INFO_H

#include <string>

namespace common {
    struct ClientInfo {
        std::string IP,       //客户端IP
        MacMd5,   //客户端MAC地址MD5
        CharName; //角色名称
    };
}


#endif //BILLING_CLIENT_INFO_H
