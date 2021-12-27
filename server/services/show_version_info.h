//
// Created by liuguang on 2021/12/26.
//

#ifndef BILLING_SHOW_VERSION_INFO_H
#define BILLING_SHOW_VERSION_INFO_H

#include "logger.h"

namespace services {
    /**
     * 展示版本信息
     * @param logger
     */
    void showVersionInfo(Logger &logger);

    /**
     * 展示编译此项目的系统类型信息
     * @param logger
     */
    void showBuilderInfo(Logger &logger);
}

#endif //BILLING_SHOW_VERSION_INFO_H
