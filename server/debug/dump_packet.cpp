//
// Created by liuguang on 2021/12/24.
//

#include <iomanip>
#include "dump_packet.h"

namespace debug {
    void dumpPacket(std::stringstream &ss, const char *title, const common::BillingPacket *packet) {
        using std::endl;
        ss << "[" << title << "] " << endl;
        ss << "==================================================" << endl;
        ss << "opType: 0x" << std::setfill('0') << std::setw(2) << std::right
           << std::hex << (int) packet->opType << endl;
        ss << "msgID: 0x" << std::setfill('0') << std::setw(4) << std::right
           << std::hex << packet->msgID << endl;
        size_t opDataSize = packet->opData.size();
        if (opDataSize != 0) {
            ss << "opData: " << std::dec << opDataSize << " bytes" << endl;
            for (std::size_t i = 0; i < opDataSize; i++) {
                ss << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) packet->opData[i]
                   << " ";
                if ((i % 16 == 15) || (i == opDataSize - 1)) {
                    ss << endl;
                } else if (i % 4 == 3) {
                    ss << " ";
                }
            }
        }
        ss << "==================================================";
    }
}