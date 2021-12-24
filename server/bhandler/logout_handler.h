//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_LOGOUT_HANDLER_H
#define BILLING_LOGOUT_HANDLER_H
#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler{
    using common::BillingPacket;
    using services::HandlerResource;

    class LogoutHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
    public:
        explicit LogoutHandler(HandlerResource *hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_LOGOUT;
        }

        void loadResponse(const BillingPacket *request, BillingPacket *response) override;
    };
}


#endif //BILLING_LOGOUT_HANDLER_H
