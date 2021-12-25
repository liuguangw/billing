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

    void RegisterHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
        PacketDataReader packetReader(&request->opData);
        //分配空间:用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        auto usernameBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username;
        PacketDataReader::buildString(username, usernameBuffer, tmpLength);
        //分配空间:超级密码
        tmpLength = packetReader.readByte();
        auto superPasswordBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(superPasswordBuffer, tmpLength);
        string superPassword;
        PacketDataReader::buildString(superPassword, superPasswordBuffer, tmpLength);
        //分配空间:密码
        tmpLength = packetReader.readByte();
        auto passwordBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(passwordBuffer, tmpLength);
        string password;
        PacketDataReader::buildString(password, passwordBuffer, tmpLength);
        //分配空间:注册IP
        tmpLength = packetReader.readByte();
        auto registerIPBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(registerIPBuffer, tmpLength);
        string registerIP;
        PacketDataReader::buildString(registerIP, registerIPBuffer, tmpLength);
        //分配空间:email
        tmpLength = packetReader.readByte();
        auto emailBuffer = new unsigned char[tmpLength];
        packetReader.readBuffer(emailBuffer, tmpLength);
        string email;
        PacketDataReader::buildString(email, emailBuffer, tmpLength);
        //
        Account account{
                .Name=username,
                .Password=password,
                .Question=superPassword,
                .Email=email};
        RegisterResult registerResult;
        unsigned char registerResultCode = 0x01;
        string registerResultText = "success";
        models::registerAccount(&registerResult, this->handlerResource->DbConn(), &account);
        if (registerResult.hasError) {
            registerResultCode = 0x04;
            registerResultText = registerResult.message;
        }
        //log
        std::stringstream ss;
        ss << "user [" << username << "](" << email << ") try to register from " << registerIP << " : "
           << registerResultText;
        this->handlerResource->logger()->infoLn(&ss);
        //
        response->opData.reserve(usernameLength + 2);
        response->opData.push_back(usernameLength);
        response->appendOpData(usernameBuffer, usernameLength);
        response->opData.push_back(registerResultCode);
        //释放分配的空间
        delete[] usernameBuffer;
        delete[] superPasswordBuffer;
        delete[] passwordBuffer;
        delete[] registerIPBuffer;
        delete[] emailBuffer;
    }
}