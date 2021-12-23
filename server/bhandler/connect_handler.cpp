//
// Created by liuguang on 2021/12/23.
//

#include "connect_handler.h"
namespace bhandler {
    using common::BillingPacket;

    void ConnectHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        response->opData.push_back((unsigned char)0x20);
        response->opData.push_back((unsigned char)0x00);
    }
}
