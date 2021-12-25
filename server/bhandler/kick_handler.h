//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_KICK_HANDLER_H
#define BILLING_KICK_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"

namespace bhandler {
    using common::BillingPacket;

    class KickHandler : public common::PacketHandler {
    public:
        unsigned char getType() override {
            return common::PACKET_TYPE_KICK;
        }

        void loadResponse(const BillingPacket *request, BillingPacket *response) override;
    };
}

#endif //BILLING_KICK_HANDLER_H
