//
// Created by liuguang on 2021/12/26.
//
#include <string>
#include <cstring>
#include <libgen.h>
#include <unistd.h>
#include "../services/server.h"
#include "run_up.h"

namespace cmd {
    int runUpCommand(int argc, char *argv[], bool disableDaemon) {
        using std::string;

        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        //获取配置文件的绝对路径
        string configFilePath = exeDir;
        configFilePath += "/config.yaml";
        //获取日志文件参数
        string logPath = "billing.log";
        bool runDaemon = false;
        for (int i = 0; i < argc; i++) {
            if ((strcmp(argv[i], "--log-path") == 0) && (i + 1 < argc)) {
                logPath = argv[i + 1];
            }
            if ((!disableDaemon) && (strcmp(argv[i], "-d") == 0)) {
                runDaemon = true;
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
        if (runDaemon) {
            //daemon - run in the background
            if (daemon(1, 1) != 0) {
                std::cerr << "run daemon failed: " << strerror(errno) << std::endl;
                return EXIT_FAILURE;
            }
        }
        server.initResource(configFilePath.c_str(), logFilePath.c_str());
        return server.run();
    }
}