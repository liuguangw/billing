//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_PACKET_HANDLER_H
#define BILLING_PACKET_HANDLER_H

#include "billing_packet.h"

namespace common {
    //billing包handler定义
    class PacketHandler {
    public:
        // 可以处理的消息类型
        virtual unsigned char getType() = 0;

        // GetResponse 根据请求获得响应
        virtual void loadResponse(const BillingPacket &request, BillingPacket &response) = 0;

        virtual ~PacketHandler() = default;
    };
}
#endif //BILLING_PACKET_HANDLER_H
