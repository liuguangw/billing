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

        // 可以处理的消息类型
        unsigned char getType() override {
            return common::PACKET_TYPE_CONNECT;
        }

        // GetResponse 根据请求获得响应,调用者负责释放内存空间
        BillingPacket *getResponse(BillingPacket *request) override;
    };
}


#endif //BILLING_CONNECT_HANDLER_H
