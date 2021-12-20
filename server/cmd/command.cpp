//
// Created by liuguang on 2021/12/19.
//
#include "command.h"
#include "../services/database_connection.h"
#include "../services/server.h"
#include <libgen.h>
#include <cstring>
#include <string>

namespace cmd {
    using std::string;
    using std::stringbuf;

    int runCommand(int argc, char *argv[]) {
        services::Server server;
        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        {
            //获取配置文件的绝对路径
            string configFilePath = exeDir;
            configFilePath += "/config.yaml";
            server.initConfig(configFilePath.c_str());
        }
        {
            string logPath = "billing.log";
            //获取日志文件参数
            for (int i = 0; i < argc; i++) {
                if ((strcmp(argv[i], "--log-path") == 0) && (i + 1 < argc)) {
                    logPath = argv[i + 1];
                    break;
                }
            }
            string logFilePath;
            //相对路径
            if (logPath[0] != '/') {
                logFilePath += exeDir;
                logFilePath += "/";
            }
            logFilePath += logPath;
            server.initLogger(logFilePath.c_str());
        }
        return server.run();
    }
}