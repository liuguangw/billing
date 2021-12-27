//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_PACKET_DATA_READER_H
#define BILLING_PACKET_DATA_READER_H

#include <vector>
#include <string>

namespace common {
    class PacketDataReader {
    private:
        std::vector<unsigned char>::const_iterator it;
        const std::vector<unsigned char> *source;
    public:
        explicit PacketDataReader(const std::vector<unsigned char> *opData);

        //读取1个字节
        unsigned char readByte();

        //读取2个字节
        unsigned short readUShort();

        //读取4个字节
        unsigned int readUint();

        //读取一部分字节
        void readBuffer(std::vector<unsigned char> &buffer, size_t readCount);

        //跳过一部分字节
        void skip(int n);

        //构造字符串
        static std::string buildString(const std::vector<unsigned char> &buffer);
    };
}


#endif //BILLING_PACKET_DATA_READER_H
