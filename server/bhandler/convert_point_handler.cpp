//
// Created by liuguang on 2021/12/25.
//

#include <sstream>
#include "convert_point_handler.h"
#include "../services/packet_data_reader.h"
#include "../common/billing_exception.h"
#include "../models/libs.h"

namespace bhandler {
    using std::string;
    using common::PacketDataReader;
    using common::BillingException;
    using models::Account;

    void ConvertPointHandler::loadResponse(const BillingPacket *request, BillingPacket *response) {
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
        //orderId 21u
        const unsigned int orderIdLength = 21;
        unsigned char orderIDBytes[orderIdLength];
        packetReader.readBuffer(orderIDBytes, orderIdLength);
        //始终为1
        unsigned short mGoodsTypeNum = packetReader.readUShort();
        //物品类型: 999表示买元宝
        unsigned int mGoodsType = packetReader.readUint();
        //物品数量(输入的点数)
        unsigned short mGoodsNumber = packetReader.readUShort();
        //需要兑换的点数
        unsigned int needPoint = mGoodsNumber;
        //每次兑换点数上限 u2
        const unsigned int maxPointLimit = 0xffff; //65535
        if (needPoint > maxPointLimit) {
            needPoint = maxPointLimit;
        }
        //初始化兑换的结果
        unsigned char convertResult = convertSuccess;
        string convertResultText = "success";
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
            convertResult = convertFailed;
            ss << "get account:" << username << " info failed: " << ex.what();
            convertResultText = ss.str();
            logger->errorLn(&ss);
        }
        if (exists) {
            userPoint = (account.Point < 0) ? 0 : (unsigned int) account.Point;
        }
        //初始化剩余点数
        unsigned int leftPoint = userPoint;
        //初始化兑换的点数
        unsigned int realPoint = needPoint;
        //兑换的点数不能超过用户拥有的点数,如果超过则截断
        if (realPoint > leftPoint) {
            realPoint = leftPoint;
        }
        //兑换的点数必须是正整数
        if (realPoint <= 0) {
            convertResult = convertFailed;
            convertResultText = "invalid need point value";
        }
        // 执行兑换
        try {
            models::convertUserPoint(dbConn, username.c_str(), realPoint);
            leftPoint -= realPoint;
        } catch (BillingException &ex) {
            convertResult = convertFailed;
            ss.str("");
            ss << "convertFailed: " << ex.what();
            convertResultText = ss.str();
            logger->errorLn(&ss);
        }
        //日志记录
        ss.str("");
        ss << "user [" << username << "] " << charName << "(ip: " << loginIP << ")"
           << "  point total [" << userPoint << "], need point [" << needPoint << "]"
           << ", (" << userPoint << " - " << realPoint << " = " << leftPoint << ")"
           << ": " << convertResultText;
        logger->infoLn(&ss);
        // 数据包组合
        response->opData.reserve(1 + usernameLength + orderIdLength + 1 + 4 + 2 + 4 + 2);
        response->opData.push_back(usernameLength);
        response->appendOpData(usernameBuffer, usernameLength);
        response->appendOpData(orderIDBytes, orderIdLength);
        response->opData.push_back(convertResult);
        //写入剩余点数:u4(此值不会被用到,服务端以购买的数量来进行计算)
        response->appendOpData(leftPoint * 1000);
        //mGoodsTypeNum:u2
        response->appendOpData(mGoodsTypeNum);
        // 写入mGoodsType:u4
        response->appendOpData(mGoodsType);
        //写入mGoodsNumber(实际购买的数量):u2
        mGoodsNumber = 0;
        if (convertResult == convertSuccess) {
            mGoodsNumber = (unsigned short) (realPoint & 0xFFFF);
        }
        response->appendOpData(mGoodsNumber);
        //释放分配的空间
        delete[] usernameBuffer;
        delete[] loginIPBuffer;
        delete[] charNameBuffer;
    }
}