//
// Created by liuguang on 2021/12/25.
//

#include "cost_log_handler.h"
#include "../services/packet_data_reader.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;

    void CostLogHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        const unsigned int mSerialKeyLength = 21;
        unsigned char mSerialKey[mSerialKeyLength];
        packetReader.readBuffer(mSerialKey, mSerialKeyLength);
        //skip zoneId(u2)
        //     +mWorldId(u4)+mServerId(u4)+mSceneId(u4)
        //     +mUserGUID(u4)+mCostTime(u4)+mYuanBao(u4)
        packetReader.skip(26);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        //auto usernameLength = tmpLength;
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
        //skip level(u2)
        packetReader.skip(2);
        //分配空间:登录IP
        tmpLength = packetReader.readByte();
        auto loginIPBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(loginIPBuffer, tmpLength);
        string loginIP;
        PacketDataReader::buildString(loginIP, loginIPBuffer, tmpLength);
        //标记在线
        common::ClientInfo clientInfo{
                .IP=loginIP,
                .CharName =  charName
        };
        services::markOnline(this->handlerResource->loginUsers(), this->handlerResource->onlineUsers(),
                             this->handlerResource->macCounters(), username.c_str(), clientInfo);
        //
        response->opData.reserve(mSerialKeyLength + 1);
        response->appendOpData(mSerialKey, mSerialKeyLength);
        response->appendOpData(common::PACKET_RESULT_SUCCESS);
        //释放分配的空间
        delete[] usernameBuffer;
        delete[] charNameBuffer;
        delete[] loginIPBuffer;
    }
}