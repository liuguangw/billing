//
// Created by liuguang on 2021/12/23.
//

#include "connect_handler.h"
namespace bhandler {
    using common::BillingPacket;

    BillingPacket *bhandler::ConnectHandler::getResponse(BillingPacket *request) {
        auto response =new BillingPacket(request);
        response->opData.push_back((unsigned char)0x20);
        response->opData.push_back((unsigned char)0x00);
        return response;
    }
}
