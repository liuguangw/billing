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

    //
    class BillingPacket {
    public:
        unsigned char opType = 0;
        unsigned short msgID = 0;
        std::vector<unsigned char> opData;

        BillingPacket() = default;

        void prepareResponse(BillingPacket* request);

        /**
         * 从数据源中读取数据
         * @param source
         * @param offset 开始的位置
         * @return 0成功 1数据包结构不完整 2数据包格式错误
         */
        unsigned int load(std::vector<unsigned char> *source,size_t offset);

        size_t fullLength() const {
            return this->opData.size() + PACKET_MIN_SIZE;
        }

        void appendOpData(unsigned int data);

        void appendOpData(unsigned short data);

        void appendOpData(std::string &data);

        //写入数据
        void putData(std::vector<unsigned char> *outputData);

        void dumpInfo(std::stringstream& ss);
    };
}


#endif //BILLING_BILLING_PACKET_H
