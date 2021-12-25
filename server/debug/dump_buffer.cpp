//
// Created by liuguang on 2021/12/24.
//

#include <iomanip>
#include "dump_buffer.h"

namespace debug {
    void dumpBuffer(std::stringstream &ss, const char *title, const unsigned char *buffer, size_t bufferSize) {
        using std::endl;
        ss << "[" << title << "] " << std::dec << bufferSize << " bytes" << endl;
        ss << "==================================================" << endl;
        for (std::size_t i = 0; i < bufferSize; i++) {
            ss << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) buffer[i]
               << " ";
            if ((i % 16 == 15) || (i == bufferSize - 1)) {
                ss << endl;
            } else if (i % 4 == 3) {
                ss << " ";
            }
        }
        ss << "==================================================";
    }
}