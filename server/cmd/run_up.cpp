//
// Created by liuguang on 2021/12/26.
//
#include <string>
#include <cstring>
#include <libgen.h>
#include <unistd.h>
#include "../services/server.h"
#include "run_up.h"
#include "../common/billing_exception.h"
#include "help_text.h"

namespace cmd {
    int runUpCommand(int argc, char *argv[], bool disableDaemon) {
        using std::string;
        //获取日志文件参数
        string logPath = "billing.log";
        //是否使用守护进程在后台运行
        bool runDaemon = false;
        bool showHelp = false;
        for (int i = 0; i < argc; i++) {
            if ((strcmp(argv[i], "--log-path") == 0) && (i + 1 < argc)) {
                logPath = argv[i + 1];
            }
            if (!disableDaemon) {
                if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--daemon") == 0)) {
                    runDaemon = true;
                }
            }
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                showHelp = true;
            }
        }
        if (showHelp) {
            if (disableDaemon) {
                std::cout << HELP_MAIN << std::endl;
            } else {
                std::cout << HELP_UP << std::endl;
            }
            return EXIT_SUCCESS;
        }
        //daemon - run in the background
        if (runDaemon) {
            if (daemon(1, 1) != 0) {
                std::cerr << "run daemon failed: " << strerror(errno) << std::endl;
                return EXIT_FAILURE;
            }
        }
        //可执行文件绝对路径
        char *exePath = canonicalize_file_name(argv[0]);
        string exeDir = dirname(exePath);
        free(exePath);
        //获取配置文件的绝对路径
        string configFilePath = exeDir;
        configFilePath += "/config.yaml";
        string logFilePath;
        //相对路径
        if (logPath[0] != '/') {
            logFilePath += exeDir;
            logFilePath += "/";
        }
        logFilePath += logPath;
        services::Server server;
        try {
            server.initResource(configFilePath.c_str(), logFilePath.c_str());
        } catch (common::BillingException &ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
        return server.run();
    }
}