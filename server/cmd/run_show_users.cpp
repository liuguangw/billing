//
// Created by liuguang on 2021/12/28.
//

#include <string>
#include <libgen.h>
#include <cstring>
#include "run_show_users.h"
#include "../common/server_config.h"
#include "../common/billing_exception.h"
#include "../services/logger.h"
#include "../services/packet_client.h"
#include "../services/packet_data_reader.h"
#include "help_text.h"

namespace cmd {
    int runShowUsers(int argc, char *argv[]) {
        using std::string;
        bool showHelp = false;
        for (int i = 0; i < argc; i++) {
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                showHelp = true;
            }
        }
        if (showHelp) {
            std::cout << HELP_SHOW_USERS << std::endl;
            return EXIT_SUCCESS;
        }
        //可执行文件绝对路径
        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        //获取配置文件的绝对路径
        string configFilePath = exeDir;
        configFilePath += "/config.yaml";
        //配置
        common::ServerConfig serverConfig;
        try {
            serverConfig.initConfig(configFilePath.c_str());
        } catch (common::BillingException &ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
        //请求包构造
        common::BillingPacket request;
        request.opType = common::PACKET_TYPE_COMMAND;
        string command = "show_users";
        request.appendOpData(command);
        //client
        services::PacketClient client(serverConfig);
        services::Logger logger;
        string responseString;
        try {
            auto response = client.execute(request);
            responseString = services::PacketDataReader::buildString(response.opData);
        } catch (common::BillingException &ex) {
            logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        //
        logger.infoLn(responseString.c_str());
        return EXIT_SUCCESS;
    }
}
