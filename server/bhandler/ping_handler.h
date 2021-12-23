//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_PING_HANDLER_H
#define BILLING_PING_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"

namespace bhandler {
    using common::BillingPacket;

    class PingHandler: public common::PacketHandler{

        unsigned char getType() override {
            return common::PACKET_TYPE_PING;
        }

        void loadResponse(const BillingPacket *request,BillingPacket *response) override;
    };
}



#endif //BILLING_PING_HANDLER_H
