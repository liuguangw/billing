//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_DUMP_PACKET_H
#define BILLING_DUMP_PACKET_H

#include <sstream>
#include "../common/billing_packet.h"

namespace debug {
    void dumpPacket(std::stringstream &ss,const char* title, const common::BillingPacket *packet);
}


#endif //BILLING_DUMP_PACKET_H
