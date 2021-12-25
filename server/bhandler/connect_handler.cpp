//
// Created by liuguang on 2021/12/23.
//

#include "connect_handler.h"

namespace bhandler {

    void ConnectHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        response->appendOpData((unsigned short) 0x2000);
    }
}
