//
// Created by liuguang on 2021/12/23.
//

#include <sstream>
#include "ping_handler.h"
#include "../services/packet_data_reader.h"

namespace bhandler {

    void PingHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        common::PacketDataReader packetReader(request.opData);
        auto zoneID = packetReader.readUShort();
        auto worldID = packetReader.readUShort();
        auto playerCount = packetReader.readUShort();
        if (playerCount != this->currentPlayerCount) {
            this->currentPlayerCount = playerCount;
            std::stringstream ss;
            ss << "server status: zoneID=" << zoneID
               << ", worldID=" << worldID
               << ", playerCount=" << playerCount;
            this->handlerResource.logger().infoLn(ss);
        }
        response.appendOpData((unsigned short) 0x0100);
    }
}