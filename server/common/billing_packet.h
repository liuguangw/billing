//
// Created by liuguang on 2021/12/22.
//

#ifndef BILLING_BILLING_PACKET_H
#define BILLING_BILLING_PACKET_H

#include <vector>
#include <string>

namespace common {
    static const unsigned char MASK0 = 0xAA;
    static const unsigned char MASK1 = 0x55;
    static const size_t PACKET_MIN_SIZE = 9;
    class BillingPacket {
    public:
        unsigned char opType = 0;
        unsigned short msgID = 0;
        std::vector<unsigned char> opData;

        BillingPacket() = default;

        /**
         * 从数据源中读取数据
         * @param source
         * @return 0成功 1数据包结构不完整 2数据包格式错误
         */
        unsigned int load(std::vector<unsigned char> *source);

        //拷贝构造函数
        BillingPacket(BillingPacket &request) : opType(request.opType), msgID(request.msgID) {}

        size_t fullLength() const {
            return this->opData.size() + PACKET_MIN_SIZE;
        }
        void appendOpData(unsigned int data);
        void appendOpData(unsigned short data);
        void appendOpData(std::string& data);
        //写入数据
        void putData(std::vector<unsigned char>* outputData);
        void dumpInfo();
    };
}


#endif //BILLING_BILLING_PACKET_H
