//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_CONNECT_HANDLER_H
#define BILLING_CONNECT_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"

namespace bhandler {
    using common::BillingPacket;

    class ConnectHandler : public common::PacketHandler {
    public:
        unsigned char getType() override {
            return common::PACKET_TYPE_CONNECT;
        }

        void loadResponse(const BillingPacket &request, BillingPacket &response) override;
    };
}


#endif //BILLING_CONNECT_HANDLER_H
