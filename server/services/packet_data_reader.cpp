//
// Created by liuguang on 2021/12/23.
//

#include "packet_data_reader.h"
#include "fill_buffer.h"

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
        auto offset = this->it - source->begin();
        size_t fillCount = services::fillBuffer(source, offset, buffer, bufferSize);
		this->it += fillCount;
		return fillCount;
    }

    void PacketDataReader::skip(int n) {
        this->it += n;
    }

    void PacketDataReader::buildString(std::string &str, unsigned char *buffer, size_t bufferSize) {
        str.reserve(bufferSize);
        for (size_t i = 0; i < bufferSize; i++) {
            str.push_back((char) buffer[i]);
        }
    }
}