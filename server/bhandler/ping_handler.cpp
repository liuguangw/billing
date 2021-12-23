//
// Created by liuguang on 2021/12/23.
//

#include "ping_handler.h"
namespace bhandler {
    using common::BillingPacket;

    void PingHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        response->opData.push_back((unsigned char)0x01);
        response->opData.push_back((unsigned char)0x00);
    }
}