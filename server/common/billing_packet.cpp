//
// Created by liuguang on 2021/12/22.
//

#include "billing_packet.h"

namespace common {
    unsigned int BillingPacket::load(std::vector<unsigned char> *source) {
        auto sourceSize = source->size();
        if (sourceSize < packetMinSize) {
            return 1;
        }
        auto it = source->begin();
        //头部检测
        if (*it != mask0) {
            return 2;
        }
        it++;
        //
        if (*it != mask1) {
            return 2;
        }
        it++;
        int dataLength = *it;
        dataLength <<= 8;
        it++;
        dataLength += (int) (*it);
        // OpData数据长度需要减去3: OpType(1字节), MsgID(2字节)
        size_t opDataLength = dataLength - 3;
        //计算包的总长度
        size_t packetFullLength = opDataLength + packetMinSize;
        //数据包不完整
        if (sourceSize < packetFullLength) {
            return 1;
        }
        it++;
        this->opType = *it;
        it++;
        this->msgID=*it;
        this->msgID<<=8;
        it++;
        this->msgID=*it;
        if(dataLength>0){
            this->opData.reserve(dataLength);
            it++;
            this->opData.insert(this->opData.begin(),it,it+dataLength);
        }
        return 0;
    }
}
