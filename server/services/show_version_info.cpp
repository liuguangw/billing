//
// Created by liuguang on 2021/12/26.
//

#include <sstream>
#include <cstring>
#include "show_version_info.h"
#include "../common/build_info.h"

namespace services {
    void showVersionInfo(Logger &logger) {
        using namespace common;
        logger.infoLn("project url: https://github.com/liuguangw/billing");
        std::stringstream ss;
        char gitHash[8];
        strncpy(gitHash, BUILD_GIT_HASH, 7);
        gitHash[7] = '\0';
        ss << "version: " << BUILD_VERSION
           << ", git-hash: " << gitHash
           << ", compiler-type: " << BUILD_COMPILER << "(" << BUILD_COMPILER_VERSION << ")"
           << ", build-time: " << BUILD_TIME;
        logger.infoLn(ss);
    }

    void showBuilderInfo(Logger &logger) {
        std::stringstream ss;
        ss << "build from " << common::BUILD_MACHINE;
        logger.infoLn(ss);
    }
}