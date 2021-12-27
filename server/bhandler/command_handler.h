//
// Created by liuguang on 2021/12/27.
//

#ifndef BILLING_COMMAND_HANDLER_H
#define BILLING_COMMAND_HANDLER_H


#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    class CommandHandler : public common::PacketHandler {
    private:
        HandlerResource &handlerResource;
    public:
        explicit CommandHandler(HandlerResource &hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_COMMAND;
        }

        void loadResponse(const BillingPacket &request, BillingPacket &response) override;
    };
}

#endif //BILLING_COMMAND_HANDLER_H
