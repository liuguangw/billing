//
// Created by liuguang on 2021/12/19.
//
#include <string>
#include <cstring>
#include <libgen.h>
#include "command.h"
#include "../services/server.h"

namespace cmd {

    int runCommand(int argc, char *argv[]) {
        using std::string;

        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        //获取配置文件的绝对路径
        string configFilePath = exeDir;
        configFilePath += "/config.yaml";
        //获取日志文件参数
        string logPath = "billing.log";
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
        services::Server server;
        server.initResource(configFilePath.c_str(),logFilePath.c_str());
        return server.run();
    }
}
