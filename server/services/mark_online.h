//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_MARK_ONLINE_H
#define BILLING_MARK_ONLINE_H

#include <map>
#include "../common/client_info.h"

namespace services {
    using common::ClientInfo;
    using std::string;

    void markOnline(std::map<string, ClientInfo> *mLoginUsers, std::map<string, ClientInfo> *mOnlineUsers,
                    std::map<string, unsigned int> *mMacCounters,
                    const char *username, ClientInfo clientInfo);
}

#endif //BILLING_MARK_ONLINE_H
