//
// Created by liuguang on 2021/12/24.
//

#include "mark_online.h"

namespace services {
    void markOnline(std::map<string, ClientInfo> &loginUsers, std::map<string, ClientInfo> &onlineUsers,
                    std::map<string, unsigned int> &macCounters,
                    const char *username, ClientInfo clientInfo) {
        //已经标记为登录了
        auto it = onlineUsers.find(username);
        if (it != onlineUsers.end()) {
            return;
        }
        //从loginUsers中删除
        it = loginUsers.find(username);
        if (it != loginUsers.end()) {
            auto &loginUserInfo = it->second;
            //补充字段信息
            clientInfo.MacMd5 = loginUserInfo.MacMd5;
            if (clientInfo.IP.empty()) {
                clientInfo.IP = loginUserInfo.IP;
            }
            loginUsers.erase(it);
        }
        //写入onlineUsers
        onlineUsers[username] = clientInfo;
        //mac计数+1
        if (!clientInfo.MacMd5.empty()) {
            unsigned int counterValue = 0;
            auto it1 = macCounters.find(clientInfo.MacMd5);
            if (it1 != macCounters.end()) {
                counterValue = it1->second;
            }
            counterValue++;
            macCounters[clientInfo.MacMd5] = counterValue;
        }
    }
}