//
// Created by liuguang on 2021/12/26.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "run_version.h"
#include "../services/logger.h"
#include "../services/show_version_info.h"
#include "help_text.h"

namespace cmd {
    int runVersionCommand(int argc, char *argv[]) {
        bool showHelp = false;
        for (int i = 0; i < argc; i++) {
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                showHelp = true;
            }
        }
        if (showHelp) {
            std::cout << HELP_VERSION << std::endl;
            return EXIT_SUCCESS;
        }
        services::Logger logger;
        services::showVersionInfo(logger);
        services::showBuilderInfo(logger);
        return EXIT_SUCCESS;
    }
}
