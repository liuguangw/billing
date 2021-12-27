//
// Created by liuguang on 2021/12/25.
//

#include <sstream>
#include "register_handler.h"
#include "../services/packet_data_reader.h"
#include "../models/libs.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;
    using models::Account;
    using models::RegisterResult;

    void RegisterHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(request.opData);
        std::vector<unsigned char> usernameBuffer, superPasswordBuffer, passwordBuffer, registerIPBuffer, emailBuffer;
        //用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //超级密码
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(superPasswordBuffer, tmpLength);
        string superPassword = PacketDataReader::buildString(superPasswordBuffer);
        //密码
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(passwordBuffer, tmpLength);
        string password = PacketDataReader::buildString(passwordBuffer);
        //注册IP
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(registerIPBuffer, tmpLength);
        string registerIP = PacketDataReader::buildString(registerIPBuffer);
        //email
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(emailBuffer, tmpLength);
        string email = PacketDataReader::buildString(emailBuffer);
        //
        Account account{
                .Name=username,
                .Password=password,
                .Question=superPassword,
                .Email=email};
        unsigned char registerResultCode = 0x01;
        string registerResultText = "success";
        auto registerResult = models::registerAccount(this->handlerResource.DbConn(), account);
        if (registerResult.hasError) {
            registerResultCode = 0x04;
            registerResultText = registerResult.message;
        }
        //log
        std::stringstream ss;
        ss << "user [" << username << "](" << email << ") try to register from " << registerIP << " : "
           << registerResultText;
        this->handlerResource.logger().infoLn(ss);
        //
        response.opData.reserve(usernameLength + 2);
        response.appendOpData(usernameLength);
        response.appendOpData(usernameBuffer);
        response.appendOpData(registerResultCode);
    }
}