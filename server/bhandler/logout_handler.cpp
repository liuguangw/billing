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

    void LogoutHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(request.opData);
        std::vector<unsigned char> usernameBuffer;
        //用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //更新在线状态
        auto &onlineUsers = this->handlerResource.onlineUsers();
        auto it = onlineUsers.find(username);
        if (it != onlineUsers.end()) {
            auto& clientInfo = it->second;
            auto& macMd5 = clientInfo.MacMd5;
            if (!macMd5.empty()) {
                auto &macCounters = this->handlerResource.macCounters();
                auto it1 = macCounters.find(macMd5);
                if (it1 != macCounters.end()) {
                    //计数器-1
                    unsigned int macCounter = it1->second;
                    if (macCounter > 0) {
                        macCounter--;
                    }
                    if (macCounter > 0) {
                        macCounters[it1->first] = macCounter;
                    } else {
                        //归0了,直接删除key
                        macCounters.erase(it1);
                    }
                }
            }
            onlineUsers.erase(it);
        }
        //logger
        services::Logger &logger = this->handlerResource.logger();
        std::stringstream ss;
        ss << "user [" << username << "] logout game";
        logger.infoLn(ss);
        //
        response.opData.reserve(usernameLength + 2);
        response.appendOpData(usernameLength);
        response.appendOpData(usernameBuffer);
        response.appendOpData(common::PACKET_RESULT_SUCCESS);
    }
}