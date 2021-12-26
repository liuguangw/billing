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

    void EnterGameHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        auto usernameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username;
        PacketDataReader::buildString(username, usernameBuffer, tmpLength);
        //分配空间:角色名
        tmpLength = packetReader.readByte();
        auto charNameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(charNameBuffer, tmpLength);
        string charName;
        PacketDataReader::buildString(charName, charNameBuffer, tmpLength);
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
        response->opData.reserve(usernameLength + 2);
        response->appendOpData(usernameLength);
        response->appendOpData(usernameBuffer, usernameLength);
        response->appendOpData(common::PACKET_RESULT_SUCCESS);
        //释放分配的空间
        delete[] usernameBuffer;
        delete[] charNameBuffer;
    }
}