//
// Created by liuguang on 2021/12/24.
//

#include <sstream>
#include "enter_game_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void EnterGameHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(&request.opData);
        std::vector<unsigned char> usernameBuffer, charNameBuffer;
        //用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //角色名
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(charNameBuffer, tmpLength);
        string charName = PacketDataReader::buildString(charNameBuffer);
        //标记在线
        common::ClientInfo clientInfo{
                .CharName =  charName
        };
        services::markOnline(this->handlerResource->loginUsers(), this->handlerResource->onlineUsers(),
                             this->handlerResource->macCounters(), username.c_str(), clientInfo);
        //logger
        auto logger = this->handlerResource->logger();
        std::stringstream ss;
        ss << "user [" << username << "] " << charName << " entered game";
        logger->infoLn(&ss);
        //
        response.opData.reserve(usernameLength + 2);
        response.appendOpData(usernameLength);
        response.appendOpData(usernameBuffer);
        response.appendOpData(common::PACKET_RESULT_SUCCESS);
    }
}