//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_LOGIN_HANDLER_H
#define BILLING_LOGIN_HANDLER_H

#include "../common/billing_packet.h"
#include "../common/packet_handler.h"
#include "../services/handler_resource.h"

namespace bhandler {
    using common::BillingPacket;
    using services::HandlerResource;

    //登录结果定义

    // 登录成功
    static const unsigned char loginCodeSuccess = 0x01;
    // 账号不存在
    static const unsigned char loginCodeNoAccount = 0x02;
    // 密码错误
    static const unsigned char loginCodeWrongPassword = 0x03;
    // 用户在线
    static const unsigned char loginCodeUserOnline = 0x04;
    // 其它错误
    static const unsigned char loginCodeOtherError = 0x06;
    // 禁止登录
    static const unsigned char loginCodeForbidden = 0x07;
    // 显示注册窗口
    static const unsigned char loginCodeShowRegister = 0x09;

    class LoginHandler : public common::PacketHandler {
    private:
        HandlerResource *handlerResource;
        bool autoReg;//自动注册
        unsigned int maxClientCount,//最多允许进入的用户数量(0表示无限制)
        pcMaxClientCount; //每台电脑最多允许进入的用户数量(0表示无限制)
    public:
        explicit LoginHandler(HandlerResource *hResource) : handlerResource(hResource) {
            auto serverConfig = hResource->config();
            this->autoReg = serverConfig->AutoReg;
            this->maxClientCount = serverConfig->MaxClientCount;
            this->pcMaxClientCount = serverConfig->PcMaxClientCount;
        }

        unsigned char getType() override {
            return common::PACKET_TYPE_LOGIN;
        }

        void loadResponse(const BillingPacket *request, BillingPacket *response) override;
    };
}


#endif //BILLING_LOGIN_HANDLER_H
