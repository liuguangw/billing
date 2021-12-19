//
// Created by liuguang on 2021/12/19.
//

#include "load_server_config.h"
#include <cstddef>
#include <vector>

namespace services {
    using std::string;
    using std::vector;

    void loadServerConfig(const char *configFilePath, common::ServerConfig *serverConfig) {
        //set default value
        serverConfig->IP = "127.0.0.1";
        serverConfig->Port = 12680;
        serverConfig->DbHost = "127.0.0.1";
        serverConfig->DbPort = 3306;
        serverConfig->DbUser = "root";
        serverConfig->DbPassword = "root";
        serverConfig->DbName = "web";
        serverConfig->AutoReg = true;
        serverConfig->MaxClientCount = 500;
        serverConfig->PcMaxClientCount = 3;
        //todo parse
    }
}