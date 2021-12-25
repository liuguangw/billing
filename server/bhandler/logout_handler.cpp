//
// Created by liuguang on 2021/12/24.
//

#include <sstream>
#include "logout_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void LogoutHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        auto usernameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username;
        PacketDataReader::buildString(username, usernameBuffer, tmpLength);
        //更新在线状态
        auto onlineUsers = this->handlerResource->onlineUsers();
        auto it = onlineUsers->find(username);
        if (it != onlineUsers->end()) {
            auto clientInfo = &it->second;
            auto macMd5 = clientInfo->MacMd5;
            if (!macMd5.empty()) {
                auto macCounters = this->handlerResource->macCounters();
                auto it1 = macCounters->find(username);
                if (it1 != macCounters->end()) {
                    //计数器-1
                    unsigned int macCounter = it1->second;
                    if (macCounter > 0) {
                        macCounter--;
                    }
                    macCounters->operator[](username) = macCounter;
                }
            }
        }
        //logger
        auto logger = this->handlerResource->logger();
        std::stringstream ss;
        ss << "user [" << username << "] logout game";
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