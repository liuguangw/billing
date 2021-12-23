//
// Created by liuguang on 2021/12/21.
//

#include "tcp_connection.h"
#include "../common/billing_exception.h"
#include "../bhandler/connect_handler.h"
#include "fill_buffer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <iomanip>
#include <cstring>

namespace services {
    using common::IoStatus;

    TcpConnection::TcpConnection(int fd) : connFd(fd) {
        this->initPacketHandlers();
        std::cout << "TcpConnection::TcpConnection fd:" << fd << std::endl;
    }

    TcpConnection::~TcpConnection() {
        std::cout << "TcpConnection::~TcpConnection" << std::endl;
        for (auto &pair: this->packetHandlers) {
            delete pair.second;
        }
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
                    return IoStatus::Pending;
                } else {
                    return IoStatus::Error;
                }
            } else if (readCount == 0) {
                return IoStatus::Disconnected;
            }
            //append
            this->inputData.insert(this->inputData.end(), buff, buff + readCount);
            //debug
            std::cout << "r====================================================" << std::endl;
            for (std::size_t i = 0; i < this->inputData.size(); i++) {
                std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) this->inputData[i]
                          << " ";
                if ((i % 16 == 15) || (i == this->inputData.size() - 1)) {
                    std::cout << std::endl;
                }
            }
            std::cout << "r====================================================" << std::endl;
        }
    }

    IoStatus TcpConnection::writeAll(unsigned char *buff) {
        size_t writeCountTotal = 0, writeCount, fillCount;
        IoStatus result = IoStatus::Ok;
        while (writeCountTotal < this->outputData.size()) {
            fillCount = fillBuffer(&this->outputData, writeCountTotal, buff, buffSize);
            if (fillCount == 0) {
                break;
            }
            writeCount = write(this->connFd, buff, fillCount);
            writeCountTotal += writeCount;
            std::cout << "writeCount: " << std::dec << writeCount << std::endl;
            if (writeCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    result = IoStatus::Pending;
                    break;
                } else {
                    return IoStatus::Error;
                }
            } else if (writeCount == 0) {
                return IoStatus::Disconnected;
            }
            //debug
            std::cout << "w====================================================" << std::endl;
            for (std::size_t i = 0; i < writeCount; i++) {
                std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) buff[i]
                          << " ";
                if ((i % 16 == 15) || (i == writeCount - 1)) {
                    std::cout << std::endl;
                }
            }
            std::cout << "w====================================================" << std::endl;
        }
        if (writeCountTotal > 0) {
            //删除左侧已经发送了的数据
            if (writeCountTotal < this->outputData.size()) {
                auto it = this->outputData.begin();
                this->outputData.erase(it, it + (int) writeCountTotal);
            } else {
                //全部发送完了,直接清空
                this->outputData.clear();
            }
        }
        return result;
    }


    void TcpConnection::addHandler(common::PacketHandler *handler) {
        this->packetHandlers[handler->getType()] = handler;
    }

    void TcpConnection::initPacketHandlers() {
        //todo add handlers
        common::PacketHandler *handler = new bhandler::ConnectHandler;
        this->addHandler(handler);
    }

    bool TcpConnection::processConn(bool readAble, bool writeAble) {
        unsigned char buffer[buffSize];
        if (readAble) {
            std::cout << "readAll" << std::endl;
            auto status = this->readAll(buffer);
            if (status != IoStatus::Pending) {
                return false;
            }
        }
        //没有数据需要处理
        if (!this->inputData.empty()) {
            common::BillingPacket request;
            common::BillingPacket *responsePtr;
            unsigned int parseResult;
            size_t parseTotalSize = 0;
            common::PacketHandler *handler;
            while (true) {
                parseResult = request.load(&this->inputData, parseTotalSize);
                if (parseResult == 1) {
                    //数据包结构不完整,跳出while解析循环
                    break;
                }
                //格式错误
                if (parseResult == 2) {
                    std::cout << "invalid packet" << std::endl;
                    return false;
                }
                parseTotalSize += request.fullLength();
                request.dumpInfo();
                try {
                    handler = this->packetHandlers.at(request.opType);
                } catch (std::out_of_range &ex) {
                    std::cout << "unknown packet opType: 0x" << std::setfill('0') << std::setw(2) << std::right
                              << std::hex << (int) request.opType << std::endl;
                    break;
                }
                responsePtr = handler->getResponse(&request);
                responsePtr->putData(&this->outputData);
                delete responsePtr;
                if (writeAble) {
                    std::cout << "writeAll.1" << std::endl;
                    auto status = this->writeAll(buffer);
                    if ((status == IoStatus::Error) || (status == IoStatus::Disconnected)) {
                        return false;
                    }
                    if (status == IoStatus::Pending) {
                        writeAble = false;
                    }
                }
            }
            if (parseTotalSize > 0) {
                //删除左侧已经解析了的数据
                if (parseTotalSize < this->inputData.size()) {
                    auto it = this->inputData.begin();
                    this->outputData.erase(it, it + (int) parseTotalSize);
                } else {
                    //全部解析完了,直接清空
                    this->inputData.clear();
                }
            }
        }
        //
        if (writeAble && !this->outputData.empty()) {
            std::cout << "writeAll.2" << std::endl;
            auto status = this->writeAll(buffer);
            if (status != IoStatus::Ok) {
                return false;
            }
        }
        return true;
    }
}