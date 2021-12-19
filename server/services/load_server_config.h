//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_LOAD_SERVER_CONFIG_H
#define BILLING_LOAD_SERVER_CONFIG_H

#include "../common/server_config.h"

namespace services {
    void loadServerConfig(const char *configFilePath, common::ServerConfig *serverConfig);
}
#endif //BILLING_LOAD_SERVER_CONFIG_H
