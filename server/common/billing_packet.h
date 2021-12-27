//
// Created by liuguang on 2021/12/22.
//

#ifndef BILLING_BILLING_PACKET_H
#define BILLING_BILLING_PACKET_H

#include <vector>
#include <iostream>
#include <string>

namespace common {
    //
    static const unsigned char MASK0 = 0xAA;
    static const unsigned char MASK1 = 0x55;
    static const size_t PACKET_MIN_SIZE = 9;
    static const unsigned char PACKET_RESULT_SUCCESS = 0x01;

    //
    static const unsigned char PACKET_TYPE_COMMAND = 0;
    static const unsigned char PACKET_TYPE_CONNECT = 0xA0;
    static const unsigned char PACKET_TYPE_PING = 0xA1;
    static const unsigned char PACKET_TYPE_LOGIN = 0xA2;
    static const unsigned char PACKET_TYPE_ENTERGAME = 0xA3;
    static const unsigned char PACKET_TYPE_LOGOUT = 0xA4;
    static const unsigned char PACKET_TYPE_KEEP = 0xA6;
    static const unsigned char PACKET_TYPE_KICK = 0xA9;
    static const unsigned char PACKET_TYPE_COSTLOG = 0xC5;
    static const unsigned char PACKET_TYPE_CONVERTPOINT = 0xE1;
    static const unsigned char PACKET_TYPE_QUERYPOINT = 0xE2;
    static const unsigned char PACKET_TYPE_REGISTER = 0xF1;

    /**
     * 解析packet的结果
     */
    enum PacketParseResult {
        PacketOk,//解析成功
        PacketNotFull,//数据包还不完整
        PacketInvalid//数据包格式错误
    };

    //
    class BillingPacket {
    public:
        unsigned char opType = 0;
        unsigned short msgID = 0;
        std::vector<unsigned char> opData;

        BillingPacket() = default;

        void prepareResponse(const BillingPacket &request);

        /**
         * 从数据源中解析数据包
         * @param source
         * @param offset 开始的位置
         * @return 解析结果
         */
        PacketParseResult loadFromSource(const std::vector<unsigned char> *source, size_t offset);

        size_t fullLength() const {
            return this->opData.size() + PACKET_MIN_SIZE;
        }

        void appendOpData(unsigned char data);

        void appendOpData(unsigned short data);

        void appendOpData(unsigned int data);

        void appendOpData(const std::string &data);

        void appendOpData(const std::vector<unsigned char> &data);

        /**
         * 打包数据,append到data后面
         * @param data
         */
        void packData(std::vector<unsigned char> &data) const;
    };
}


#endif //BILLING_BILLING_PACKET_H
