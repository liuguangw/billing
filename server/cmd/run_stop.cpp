//
// Created by liuguang on 2021/12/26.
//

#include <cstdlib>
#include <string>
#include <libgen.h>
#include "run_stop.h"
#include "../services/logger.h"
#include "../common/server_config.h"
#include "../services/load_server_config.h"
#include "../services/packet_client.h"
#include "../common/billing_exception.h"
#include "../debug/dump_packet.h"

namespace cmd {
    int runStopCommand(char *argv[]) {
        using std::string;
        //可执行文件绝对路径
        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        //获取配置文件的绝对路径
        string configFilePath = exeDir;
        configFilePath += "/config.yaml";
        //配置
        common::ServerConfig serverConfig;
        services::loadServerConfig(configFilePath.c_str(), &serverConfig);
        //请求包构造
        common::BillingPacket request;
        request.opType = common::PACKET_TYPE_COMMAND;
        string command = "stop";
        request.appendOpData(command);
        //client
        services::PacketClient client(serverConfig);
        services::Logger logger;
        try {
            auto response = client.execute(request);
            logger.infoLn("stop command sent successfully");
            //todo debug
            std::stringstream ss;
            debug::dumpPacket(ss, "response", &response);
            logger.infoLn(ss);
        } catch (common::BillingException &ex) {
            logger.errorLn(ex.what());
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
