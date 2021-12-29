//
// Created by liuguang on 2021/12/26.
//

#include <cstdlib>
#include <string>
#include <libgen.h>
#include <cstring>
#include "run_stop.h"
#include "../common/server_config.h"
#include "../common/billing_exception.h"
#include "../services/logger.h"
#include "../services/packet_client.h"
#include "help_text.h"

namespace cmd {
    int runStopCommand(int argc, char *argv[]) {
        using std::string;
        bool showHelp = false;
        for (int i = 0; i < argc; i++) {
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                showHelp = true;
            }
        }
        if (showHelp) {
            std::cout << HELP_STOP << std::endl;
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
        string command = "stop";
        request.appendOpData(command);
        //client
        services::PacketClient client(serverConfig);
        services::Logger logger;
        try {
            client.execute(request);
            logger.infoLn("stop command sent successfully");
        } catch (common::BillingException &ex) {
            logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
