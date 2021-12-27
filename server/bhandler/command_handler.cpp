//
// Created by liuguang on 2021/12/27.
//

#include "command_handler.h"
#include "login_handler.h"
#include "../services/packet_data_reader.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void CommandHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        string command = PacketDataReader::buildString(request.opData);
        auto &logger = this->handlerResource.logger();
        if (command == "show_users") {
            response.appendOpData("todo");
            logger.infoLn("todo show_users");
        } else {
            //stop
            this->handlerResource.markStop(true);
            response.appendOpData(common::PACKET_RESULT_SUCCESS);
        }
    }
}