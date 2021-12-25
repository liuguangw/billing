//
// Created by liuguang on 2021/12/19.
//

#include "load_server_config.h"
#include <yaml-cpp/yaml.h>
#include <string>

namespace services {
    using std::string;

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
        //parse
        auto config = YAML::LoadFile(configFilePath);
        if (config["ip"]) {
            serverConfig->IP = config["ip"].as<string>();
        }
        if (config["port"]) {
            serverConfig->Port = config["port"].as<int>();
        }
        if (config["db_host"]) {
            serverConfig->DbHost = config["db_host"].as<string>();
        }
        if (config["db_port"]) {
            serverConfig->DbPort = config["db_port"].as<int>();
        }
        if (config["db_user"]) {
            serverConfig->DbUser = config["db_user"].as<string>();
        }
        if (config["db_password"]) {
            serverConfig->DbPassword = config["db_password"].as<string>();
        }
        if (config["db_name"]) {
            serverConfig->DbName = config["db_name"].as<string>();
        }
        if (config["allow_old_password"]) {
            serverConfig->AllowOldPassword = config["allow_old_password"].as<bool>();
        }
        if (config["auto_reg"]) {
            serverConfig->AutoReg = config["auto_reg"].as<bool>();
        }
        auto allowIps = config["allow_ips"];
        if (allowIps) {
            serverConfig->AllowIps.clear();
            for (auto it = allowIps.begin(); it != allowIps.end(); ++it) {
                serverConfig->AllowIps.push_back(it->as<string>());
            }
        }
        if (config["max_client_count"]) {
            serverConfig->MaxClientCount = config["max_client_count"].as<int>();
        }
        if (config["pc_max_client_count"]) {
            serverConfig->PcMaxClientCount = config["pc_max_client_count"].as<int>();
        }
    }
}