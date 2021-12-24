//
// Created by liuguang on 2021/12/22.
//

#include <iostream>
#include "billing_packet.h"

namespace common {

    void BillingPacket::prepareResponse(const BillingPacket *request) {
        this->opType = request->opType;
        this->msgID = request->msgID;
        this->opData.clear();
    }

    PacketParseResult BillingPacket::loadFromSource(const std::vector<unsigned char> *source, size_t offset) {
        auto sourceSize = source->size() - offset;
        if (sourceSize < PACKET_MIN_SIZE) {
            return PacketParseResult::PacketNotFull;
        }
        auto it = source->begin() + (int) offset;
        //头部检测
        if (*it != MASK0) {
            return PacketParseResult::PacketInvalid;
        }
        it++;
        //
        if (*it != MASK1) {
            return PacketParseResult::PacketInvalid;
        }
        it++;
        //
        int dataLength = *it;
        dataLength <<= 8;
        it++;
        dataLength += (int) (*it);
        it++;
        // OpData数据长度需要减去3: OpType(1字节), MsgID(2字节)
        size_t opDataLength = dataLength - 3;
        //计算包的总长度
        size_t packetFullLength = opDataLength + PACKET_MIN_SIZE;
        //数据包不完整
        if (sourceSize < packetFullLength) {
            return PacketParseResult::PacketNotFull;
        }
        this->opType = *it;
        it++;
        this->msgID = *it;
        this->msgID <<= 8;
        it++;
        this->msgID += *it;
        it++;
        this->opData.clear();
        if (opDataLength > 0) {
            this->opData.reserve(opDataLength);
            this->opData.insert(this->opData.begin(), it, it + (int) opDataLength);
        }
        return PacketParseResult::PacketOk;
    }

    void BillingPacket::appendToOutput(std::vector<unsigned char> *outputData) {
        //保证空间大小足够
        outputData->reserve(outputData->size() + this->fullLength());
        outputData->push_back(MASK0);
        outputData->push_back(MASK1);
        unsigned int lengthP = 3 + this->opData.size();
        outputData->push_back((unsigned char) (lengthP >> 8));
        outputData->push_back((unsigned char) (lengthP & 0xff));
        outputData->push_back(this->opType);
        outputData->push_back((unsigned char) (this->msgID >> 8));
        outputData->push_back((unsigned char) (this->msgID & 0xff));
        if (!this->opData.empty()) {
            outputData->insert(outputData->end(), this->opData.begin(), this->opData.end());
        }
        outputData->push_back(MASK1);
        outputData->push_back(MASK0);
    }

    void BillingPacket::appendOpData(unsigned int data) {
        unsigned int tmp;
        for (auto i = 0; i < 4; i++) {
            tmp = data;
            if (3 - i > 0) {
                tmp >>= (8 * (3 - i));
            }
            tmp &= 0xFF;
            this->opData.push_back((unsigned char) tmp);
        }
    }

    void BillingPacket::appendOpData(unsigned short data) {
        unsigned short tmp = data;
        tmp >>= 8;
        tmp &= 0xFF;
        this->opData.push_back((unsigned char) tmp);
        tmp = data & 0xFF;
        this->opData.push_back((unsigned char) tmp);
    }

    void BillingPacket::appendOpData(const std::string &data) {
        for (char it: data) {
            this->opData.push_back((unsigned char) it);
        }
    }

    void BillingPacket::appendOpData(const unsigned char *data, size_t dataSize) {
        this->opData.insert(this->opData.end(), data, data + dataSize);
    }
}
