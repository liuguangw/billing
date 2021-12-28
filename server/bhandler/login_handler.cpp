//
// Created by liuguang on 2021/12/23.
//

#include <sstream>
#include "login_handler.h"
#include "../services/packet_data_reader.h"
#include "../models/libs.h"

namespace bhandler {
    using std::string;
    using services::PacketDataReader;

    void LoginHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        PacketDataReader packetReader(request.opData);
        std::vector<unsigned char> usernameBuffer, passwordBuffer, loginIPBuffer, macMd5Buffer;
        //用户名
        auto tmpLength = packetReader.readByte();
        auto usernameLength = tmpLength;
        packetReader.readBuffer(usernameBuffer, tmpLength);
        string username = PacketDataReader::buildString(usernameBuffer);
        //密码
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(passwordBuffer, tmpLength);
        string password = PacketDataReader::buildString(passwordBuffer);
        //登录IP
        tmpLength = packetReader.readByte();
        packetReader.readBuffer(loginIPBuffer, tmpLength);
        string loginIP = PacketDataReader::buildString(loginIPBuffer);
        //跳过level,密码卡数据
        packetReader.skip(2 + 6 + 6);
        //macMd5
        const unsigned int macMd5BufferSize = 32;
        packetReader.readBuffer(macMd5Buffer, macMd5BufferSize);
        string macMd5 = PacketDataReader::buildString(macMd5Buffer);
        //初始化
        unsigned char loginResultCode = loginCodeSuccess;
        auto loginResult = models::checkLogin(this->handlerResource.DbConn(), this->handlerResource.onlineUsers(),
                                              username.c_str(), password.c_str());
        string loginResultDescription = loginResult.message;
        switch (loginResult.loginError) {
            case models::LoginNoError:
                break;
            case models::LoginUserNotFound:
                loginResultCode = loginCodeNoAccount;
                break;
            case models::LoginInvalidPassword:
                loginResultCode = loginCodeWrongPassword;
                break;
            case models::LoginAccountLocked:
                loginResultCode = loginCodeForbidden;
                break;
            case models::LoginAccountOnline:
                loginResultCode = loginCodeUserOnline;
                break;
            case models::LoginOtherError:
                loginResultCode = loginCodeOtherError;
                break;
        }
        // 如果开启了自动注册
        if (loginResultCode == loginCodeNoAccount && this->autoReg) {
            loginResultCode = loginCodeShowRegister;
            loginResultDescription = "show register dialog";
        }

        //判断连接的用户数是否达到限制
        if (loginResultCode == loginCodeSuccess && this->maxClientCount > 0) {
            auto currentCount = (unsigned int) this->handlerResource.onlineUsers().size();
            if (currentCount >= this->maxClientCount) {
                loginResultCode = loginCodeOtherError;
                loginResultDescription = "reach max_client_count limit";
            }
        }
        //判断此电脑的连接数是否达到限制
        if (loginResultCode == loginCodeSuccess && this->pcMaxClientCount > 0) {
            unsigned int macCounter = 0;
            auto &macCounters = this->handlerResource.macCounters();
            auto it = macCounters.find(macMd5);
            if (it != macCounters.end()) {
                macCounter = it->second;
            }
            if (macCounter >= this->pcMaxClientCount) {
                loginResultCode = loginCodeOtherError;
                loginResultDescription = "reach pc_max_client_count limit";
            }
        }
        //将用户信息写入LoginUsers
        if ((loginResultCode == loginCodeSuccess) || (loginResultCode == loginCodeShowRegister)) {
            common::ClientInfo clientInfo{
                    .IP = loginIP,
                    .MacMd5 = macMd5};
            auto &loginUsers = this->handlerResource.loginUsers();
            loginUsers[username] = clientInfo;
        }
        //logger
        auto &logger = this->handlerResource.logger();
        std::stringstream ss;
        ss << "user [" << username << "] try to login from " << loginIP << "(Mac_md5=" << macMd5 << ") : "
           << loginResultDescription;
        logger.infoLn(ss);
        //
        response.opData.reserve(usernameLength + 2);
        response.appendOpData(usernameLength);
        response.appendOpData(usernameBuffer);
        response.appendOpData(loginResultCode);
    }
}
