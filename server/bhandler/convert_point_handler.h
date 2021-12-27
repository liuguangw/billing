//
// Created by liuguang on 2021/12/25.
//

#ifndef BILLING_CONVERT_POINT_HANDLER_H
#define BILLING_CONVERT_POINT_HANDLER_H


#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;
    static const unsigned char convertSuccess = 0; //兑换成功
    static const unsigned char convertFailed = 1; //兑换失败

    class ConvertPointHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
    public:
        explicit ConvertPointHandler(HandlerResource *hResource) : handlerResource(hResource) {
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_CONVERTPOINT;
        }

        void loadResponse(const BillingPacket &request, BillingPacket &response) override;
    };
}


#endif //BILLING_CONVERT_POINT_HANDLER_H
