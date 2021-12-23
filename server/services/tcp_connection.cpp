//
// Created by liuguang on 2021/12/21.
//

#include "tcp_connection.h"
#include "../common/billing_exception.h"
#include "../common/billing_packet.h"
#include "fill_buffer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <csignal>
#include <iomanip>
#include <cstring>

namespace services {
    using common::IoStatus;

    TcpConnection::~TcpConnection() {
        std::cout << "TcpConnection::~TcpConnection" << std::endl;
        close(this->connFd);
        std::cout << "TcpConnection destroy fd:" << this->connFd << std::endl;
    }

    IoStatus TcpConnection::readAll(unsigned char *buff) {
        ssize_t readCount;
        while (true) {
            readCount = read(this->connFd, buff, buffSize);
            std::cout << "readCount: " << std::dec << readCount << std::endl;
            if (readCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    return IoStatus::Ok;
                } else {
                    return IoStatus::Error;
                }
            } else if (readCount == 0) {
                return IoStatus::Disconnected;
            }
            //append
            this->inputData.insert(this->inputData.end(), buff, buff + readCount);
            //debug
            std::cout << "====================================================" << std::endl;
            for (std::size_t i = 0; i < this->inputData.size(); i++) {
                std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) this->inputData[i]
                          << " ";
                if ((i % 16 == 15) || (i == this->inputData.size() - 1)) {
                    std::cout << std::endl;
                }
            }
            std::cout << "====================================================" << std::endl;
        }
    }

    common::IoStatus TcpConnection::writeAll(unsigned char *buff) {
        size_t writeCountTotal = 0, writeCount, fillCount;
        while (writeCountTotal < this->outputData.size()) {
            fillCount = fillBuffer(&this->outputData, writeCountTotal, buff, buffSize);
            if (fillCount == 0) {
                break;
            }
            writeCount = write(this->connFd, buff, fillCount);
            writeCountTotal += writeCount;
            std::cout << "writeCount: " << writeCount << std::endl;
            if (writeCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    break;
                } else {
                    return IoStatus::Error;
                }
            } else if (writeCount == 0) {
                return IoStatus::Disconnected;
            }
        }
        if (writeCountTotal > 0) {
            //删除左侧已经发送了的数据
            if (writeCountTotal < this->outputData.size()) {
                auto it = this->outputData.begin();
                this->outputData.erase(it, it + writeCountTotal);
            } else {
                //全部发送完了,直接清空
                this->outputData.clear();
            }
        }
        return IoStatus::Ok;
    }

    bool TcpConnection::processConn(bool readAble, bool writeAble) {
        unsigned char buffer[buffSize];
        if (readAble) {
            std::cout << "readAll" << std::endl;
            auto status = this->readAll(buffer);
            if (status != IoStatus::Ok) {
                return false;
            }
        }
        //没有数据需要处理
        if (this->inputData.empty()) {
            std::cout << "nothing need process" << std::endl;
            common::BillingPacket packet;
            packet.opType=0xE5;
            packet.msgID=0x1234;
            const char* msg="112233";
            for(int i=0;i<strlen(msg);i++){
                packet.opData.push_back((unsigned char)msg[i]);

            }
            packet.dumpInfo();
            return true;
        }
        //
        std::cout << "process" << std::endl;
        this->outputData.push_back('[');
        this->outputData.insert(this->outputData.end(), this->inputData.begin(), this->inputData.end());
        this->outputData.push_back(']');
        this->inputData.clear();
        //
        if (writeAble) {
            std::cout << "writeAll" << std::endl;
            auto status = this->writeAll(buffer);
            if (status != IoStatus::Ok) {
                return false;
            }
        }
        return true;
    }
}