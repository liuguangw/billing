//
// Created by liuguang on 2021/12/23.
//

#include "packet_data_reader.h"

namespace common {

    PacketDataReader::PacketDataReader(const std::vector<unsigned char> *opData) : it(opData->begin()), source(opData) {
    }

    unsigned char PacketDataReader::readByte() {
        unsigned char temp = *this->it;
        this->it++;
        return temp;
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
        unsigned int targetValue = 0, tmpValue;
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

    void PacketDataReader::readBuffer(std::vector<unsigned char> &buffer, size_t readCount) {
        auto availableCount = (size_t) (this->source->end() - this->it);
        size_t nCount = readCount > availableCount ? availableCount : readCount;
        auto end = this->it + (int) nCount;
        buffer.reserve(buffer.size() + nCount);
        buffer.insert(buffer.end(), this->it, end);
        this->it = end;
    }

    void PacketDataReader::skip(int n) {
        this->it += n;
    }

    std::string PacketDataReader::buildString(const std::vector<unsigned char> &buffer) {
        std::string str;
        str.reserve(buffer.size());
        const char *pos = (const char *) buffer.data();
        str.append(pos, pos + buffer.size());
        return str;
    }
}