//
// Created by liuguang on 2021/12/28.
//

#include <yaml-cpp/yaml.h>
#include "server_config.h"
#include "billing_exception.h"

namespace common {

    ServerConfig::ServerConfig() : IP("127.0.0.1"), Port(12680),
                                   DbHost("127.0.0.1"), DbPort(3306),
                                   DbUser("root"), DbPassword("root"),
                                   DbName("web"), AllowOldPassword(false), AutoReg(true),
                                   MaxClientCount(500), PcMaxClientCount(3) {

    }

    void ServerConfig::initConfig(const char *configFilePath) {
        //parse
        try {
            auto config = YAML::LoadFile(configFilePath);
            if (config["ip"]) {
                this->IP = config["ip"].as<string>();
            }
            if (config["port"]) {
                this->Port = config["port"].as<unsigned short>();
            }
            if (config["db_host"]) {
                this->DbHost = config["db_host"].as<string>();
            }
            if (config["db_port"]) {
                this->DbPort = config["db_port"].as<unsigned int>();
            }
            if (config["db_user"]) {
                this->DbUser = config["db_user"].as<string>();
            }
            if (config["db_password"]) {
                this->DbPassword = config["db_password"].as<string>();
            }
            if (config["db_name"]) {
                this->DbName = config["db_name"].as<string>();
            }
            if (config["allow_old_password"]) {
                this->AllowOldPassword = config["allow_old_password"].as<bool>();
            }
            if (config["auto_reg"]) {
                this->AutoReg = config["auto_reg"].as<bool>();
            }
            auto allowIps = config["allow_ips"];
            if (allowIps) {
                this->AllowIps.clear();
                for (auto it = allowIps.begin(); it != allowIps.end(); ++it) {
                    this->AllowIps.push_back(it->as<string>());
                }
            }
            if (config["max_client_count"]) {
                this->MaxClientCount = config["max_client_count"].as<unsigned int>();
            }
            if (config["pc_max_client_count"]) {
                this->PcMaxClientCount = config["pc_max_client_count"].as<unsigned int>();
            }
        } catch (YAML::Exception &ex) {
            throw BillingException("load config failed", ex.what());
        }
    }
}