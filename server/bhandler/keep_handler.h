//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_KEEP_HANDLER_H
#define BILLING_KEEP_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    class KeepHandler : public common::PacketHandler {
    public:
        explicit KeepHandler(HandlerResource &hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_KEEP;
        }

        void loadResponse(const BillingPacket &request, BillingPacket &response) override;

    private:
        HandlerResource &handlerResource;
    };
}


#endif //BILLING_KEEP_HANDLER_H
