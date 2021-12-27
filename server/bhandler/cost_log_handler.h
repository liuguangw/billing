//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_COST_LOG_HANDLER_H
#define BILLING_COST_LOG_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    class CostLogHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
    public:
        explicit CostLogHandler(HandlerResource *hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_COSTLOG;
        }

        void loadResponse(const BillingPacket &request, BillingPacket &response) override;
    };
}

#endif //BILLING_COST_LOG_HANDLER_H
