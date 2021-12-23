//
// Created by liuguang on 2021/12/23.
//

#ifndef BILLING_PACKET_DATA_READER_H
#define BILLING_PACKET_DATA_READER_H

#include <vector>

namespace common {
    class PacketDataReader {
    private:
        std::vector<unsigned char>::const_iterator it;
        const std::vector<unsigned char> *source;
    public:
        explicit PacketDataReader(const std::vector<unsigned char> *opData);

        //读取2个字节
        unsigned short readUShort();

        //读取4个字节
        unsigned int readUint();

        //读取一部分字节
        size_t readBuffer(unsigned char* buffer,size_t bufferSize);

        //跳过一部分字节
        void skip(int n);
    };
}


#endif //BILLING_PACKET_DATA_READER_H
