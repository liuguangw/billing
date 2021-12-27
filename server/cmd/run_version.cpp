//
// Created by liuguang on 2021/12/26.
//

#include <cstdlib>
#include "run_version.h"
#include "../services/logger.h"
#include "../services/show_version_info.h"

namespace cmd {
    int runVersionCommand() {
        services::Logger logger;
        services::showVersionInfo(logger);
        services::showBuilderInfo(logger);
        return EXIT_SUCCESS;
    }
}
