//
// Created by liuguang on 2021/12/25.
//

#include <sstream>
#include "query_point_handler.h"
#include "../services/packet_data_reader.h"
#include "../common/billing_exception.h"
#include "../models/libs.h"
#include "../services/mark_online.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;
    using common::BillingException;
    using models::Account;

    void QueryPointHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        auto usernameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username;
        PacketDataReader::buildString(username, usernameBuffer, tmpLength);
        //分配空间:登录IP
        tmpLength = packetReader.readByte();
        auto loginIPBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(loginIPBuffer, tmpLength);
        string loginIP;
        PacketDataReader::buildString(loginIP, loginIPBuffer, tmpLength);
        //分配空间:角色名
        tmpLength = packetReader.readByte();
        auto charNameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(charNameBuffer, tmpLength);
        string charName;
        PacketDataReader::buildString(charName, charNameBuffer, tmpLength);
        //查询数据库获取用户当前点数余额
        unsigned int userPoint = 0;
        Account account{};
        bool exists = false;
        auto dbConn = this->handlerResource->DbConn();
        auto logger = this->handlerResource->logger();
        std::stringstream ss;
        try {
            models::loadAccountByUsername(dbConn, username.c_str(), &account, &exists);
        } catch (BillingException &ex) {
            ss << "get account:" << username << " info failed: " << ex.what();
            logger->errorLn(&ss);
        }
        if (exists) {
            userPoint = (account.Point < 0) ? 0 : (unsigned int) account.Point;
        }
        //标记在线
        common::ClientInfo clientInfo{
                .IP=loginIP,
                .CharName=charName};
        services::markOnline(this->handlerResource->loginUsers(), this->handlerResource->onlineUsers(),
                             this->handlerResource->macCounters(), username.c_str(), clientInfo);
        unsigned int pointValue = (userPoint + 1) * 1000;
        response->opData.reserve(usernameLength + 5);
        response->opData.push_back(usernameLength);
        response->appendOpData(usernameBuffer, usernameLength);
        response->appendOpData(pointValue);
        //释放分配的空间
        delete[] usernameBuffer;
        delete[] loginIPBuffer;
        delete[] charNameBuffer;
    }
}