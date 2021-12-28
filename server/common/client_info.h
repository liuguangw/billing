//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_CLIENT_INFO_H
#define BILLING_CLIENT_INFO_H

#include <string>
#include <ostream>

namespace common {
    struct ClientInfo {
        //客户端IP
        std::string IP,
        //客户端MAC地址MD5
        MacMd5,
        //角色名称
        CharName;

        friend std::ostream &operator<<(std::ostream &stream, const ClientInfo &clientInfo) {
            stream << "{ip=" << clientInfo.IP
                   << ", mac_md5=" << clientInfo.MacMd5
                   << ", char_name=" << clientInfo.CharName
                   << "}";
            return stream;
        }
    };
}


#endif //BILLING_CLIENT_INFO_H
