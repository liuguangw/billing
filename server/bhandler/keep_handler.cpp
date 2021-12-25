//
// Created by liuguang on 2021/12/25.
//

#include <sstream>
#include "keep_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void KeepHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        auto usernameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username;
        PacketDataReader::buildString(username, usernameBuffer, tmpLength);
        //等级
        auto playerLevel = packetReader.readUShort();
        //标记在线
        common::ClientInfo clientInfo{};
        services::markOnline(this->handlerResource->loginUsers(), this->handlerResource->onlineUsers(),
                             this->handlerResource->macCounters(), username.c_str(), clientInfo);
        //logger
        auto logger = this->handlerResource->logger();
        std::stringstream ss;
        ss << "keep: user [" << username << "] level " << playerLevel;
        logger->infoLn(&ss);
        //
        response->opData.reserve(usernameLength + 2);
        response->opData.push_back(usernameLength);
        response->appendOpData(usernameBuffer, usernameLength);
        response->opData.push_back(0x01);
        //释放分配的空间
        delete[] usernameBuffer;
    }
}