//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_DUMP_BUFFER_H
#define BILLING_DUMP_BUFFER_H

#include <sstream>

namespace debug {
    void dumpBuffer(std::stringstream &ss, const char *title, const unsigned char *buffer, size_t bufferSize);
}

#endif //BILLING_DUMP_BUFFER_H
