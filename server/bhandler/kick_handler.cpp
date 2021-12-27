//
// Created by liuguang on 2021/12/25.
//

#include "kick_handler.h"

namespace bhandler {

    void KickHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        response.appendOpData(common::PACKET_RESULT_SUCCESS);
    }
}