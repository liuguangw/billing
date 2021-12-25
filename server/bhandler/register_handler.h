//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_REGISTER_HANDLER_H
#define BILLING_REGISTER_HANDLER_H


#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    class RegisterHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
    public:
        explicit RegisterHandler(HandlerResource *hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_REGISTER;
        }

        void loadResponse(const BillingPacket *request, BillingPacket *response) override;
    };
}


#endif //BILLING_REGISTER_HANDLER_H
