//
// Created by liuguang on 2021/12/25.
//

#include "cost_log_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void CostLogHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(request.opData);
        std::vector<unsigned char> mSerialKeyBuffer, usernameBuffer, charNameBuffer, loginIPBuffer;
        const unsigned int mSerialKeyLength = 21;
        packetReader.readBuffer(mSerialKeyBuffer, mSerialKeyLength);
        //skip zoneId(u2)
        //     +mWorldId(u4)+mServerId(u4)+mSceneId(u4)
        //     +mUserGUID(u4)+mCostTime(u4)+mYuanBao(u4)
        packetReader.skip(26);
        //用户名
        auto tmpLength = packetReader.readByte();
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //角色名
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(charNameBuffer, tmpLength);
        string charName = PacketDataReader::buildString(charNameBuffer);
        //skip level(u2)
        packetReader.skip(2);
        //登录IP
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(loginIPBuffer, tmpLength);
        string loginIP = PacketDataReader::buildString(loginIPBuffer);
        //标记在线
        common::ClientInfo clientInfo{
                .IP=loginIP,
                .CharName =  charName
        };
        services::markOnline(this->handlerResource.loginUsers(), this->handlerResource.onlineUsers(),
                             this->handlerResource.macCounters(), username.c_str(), clientInfo);
        //
        response.opData.reserve(mSerialKeyLength + 1);
        response.appendOpData(mSerialKeyBuffer);
        response.appendOpData(common::PACKET_RESULT_SUCCESS);
    }
}