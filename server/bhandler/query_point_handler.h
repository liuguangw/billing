//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_QUERY_POINT_HANDLER_H
#define BILLING_QUERY_POINT_HANDLER_H


#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    class QueryPointHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
    public:
        explicit QueryPointHandler(HandlerResource *hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_QUERYPOINT;
        }

        void loadResponse(const BillingPacket *request, BillingPacket *response) override;
    };
}


#endif //BILLING_QUERY_POINT_HANDLER_H
