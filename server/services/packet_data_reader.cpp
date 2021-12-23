//
// Created by liuguang on 2021/12/23.
//

#include "packet_data_reader.h"
#include "fill_buffer.h"

namespace common {

    PacketDataReader::PacketDataReader(const std::vector<unsigned char> *opData) : it(opData->begin()), source(opData) {
    }

    unsigned short PacketDataReader::readUShort() {
        unsigned short temp = *this->it;
        temp <<= 8;
        this->it++;
        temp += *this->it;
        this->it++;
        return temp;
    }

    unsigned int PacketDataReader::readUint() {
        int targetValue = 0, tmpValue;
        for (int i = 0; i < 4; i++) {
            tmpValue = *this->it;
            this->it++;
            if (3 - i > 0) {
                tmpValue <<= ((3 - i) * 8);
            }
            targetValue += tmpValue;
        }
        return targetValue;
    }

    size_t PacketDataReader::readBuffer(unsigned char *buffer, size_t bufferSize) {
        auto offset = this->it-source->begin();
        return services::fillBuffer(source,offset,buffer,bufferSize);
    }

    void PacketDataReader::skip(int n) {
        this->it += n;
    }
}