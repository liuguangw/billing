//
// Created by liuguang on 2021/12/25.
//

#include <sstream>
#include "keep_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"
#include "../services/logger.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void KeepHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(request.opData);
        std::vector<unsigned char> usernameBuffer;
        //用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //等级
        auto playerLevel = packetReader.readUShort();
        //标记在线
        common::ClientInfo clientInfo{};
        services::markOnline(this->handlerResource.loginUsers(), this->handlerResource.onlineUsers(),
                             this->handlerResource.macCounters(), username.c_str(), clientInfo);
        //logger
        auto &logger = this->handlerResource.logger();
        std::stringstream ss;
        ss << "keep: user [" << username << "] level " << playerLevel;
        logger.infoLn(ss);
        //
        response.opData.reserve(usernameLength + 2);
        response.appendOpData(usernameLength);
        response.appendOpData(usernameBuffer);
        response.appendOpData(common::PACKET_RESULT_SUCCESS);
    }
}