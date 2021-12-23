//
// Created by liuguang on 2021/12/21.
//

#include "tcp_connection.h"
#include "../common/billing_exception.h"
#include "../bhandler/connect_handler.h"
#include "../bhandler/ping_handler.h"
#include "fill_buffer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <iomanip>
#include <cstring>

namespace services {
    using common::IoStatus;

    TcpConnection::TcpConnection(int fd, Logger *logger) : connFd(fd), logger(logger) {
        this->initPacketHandlers();
        std::stringstream ss;
        ss << "TcpConnection::TcpConnection fd:" << fd;
        logger->infoLn(&ss);
    }

    TcpConnection::~TcpConnection() {
        this->logger->infoLn("TcpConnection::~TcpConnection");
        for (auto &pair: this->packetHandlers) {
            delete pair.second;
        }
        close(this->connFd);
        std::stringstream ss;
        ss<< "TcpConnection destroy fd:" << this->connFd ;
        logger->infoLn(&ss);
    }

    IoStatus TcpConnection::readAll(unsigned char *buff) {
        ssize_t readCount;
        //debug
        std::stringstream ss;
        while (true) {
            readCount = read(this->connFd, buff, buffSize);
            ss << "readCount: " << std::dec << readCount;
            this->logger->infoLn(&ss);
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
            this->logger->infoLn("read start==========================================");
            ss.str("");
            for (std::size_t i = 0; i < this->inputData.size(); i++) {
                ss << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) this->inputData[i]
                          << " ";
                if ((i % 16 == 15) || (i == this->inputData.size() - 1)) {
                    this->logger->infoLn(&ss);
                    ss.str("");
                }
            }
            this->logger->infoLn("read end============================================");
        }
    }

    IoStatus TcpConnection::writeAll(unsigned char *buff) {
        size_t writeCountTotal = 0, writeCount, fillCount;
        IoStatus result = IoStatus::Ok;
        //debug
        std::stringstream ss;
        while (writeCountTotal < this->outputData.size()) {
            fillCount = fillBuffer(&this->outputData, writeCountTotal, buff, buffSize);
            if (fillCount == 0) {
                break;
            }
            writeCount = write(this->connFd, buff, fillCount);
            writeCountTotal += writeCount;
            ss << "writeCount: " << std::dec << writeCount;
            this->logger->infoLn(&ss);
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
            this->logger->infoLn("write start==========================================");
            ss.str("");
            for (std::size_t i = 0; i < writeCount; i++) {
                ss << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) buff[i]
                   << " ";
                if ((i % 16 == 15) || (i == writeCount - 1)) {
                    this->logger->infoLn(&ss);
                    ss.str("");
                }
            }
            this->logger->infoLn("write end============================================");
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
        handler = new bhandler::PingHandler;
        this->addHandler(handler);
    }

    bool TcpConnection::processConn(bool readAble, bool writeAble) {
        unsigned char buffer[buffSize];
        //debug
        std::stringstream ss;
        if (readAble) {
            this->logger->infoLn("readAll");
            auto status = this->readAll(buffer);
            if (status != IoStatus::Pending) {
                return false;
            }
        }
        //没有数据需要处理
        if (!this->inputData.empty()) {
            common::BillingPacket request, response;
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
                    this->logger->errorLn("invalid packet");
                    return false;
                }
                parseTotalSize += request.fullLength();
                request.dumpInfo();
                try {
                    handler = this->packetHandlers.at(request.opType);
                } catch (std::out_of_range &ex) {
                    ss.str("");
                    ss<< "unknown packet opType: 0x" << std::setfill('0') << std::setw(2) << std::right
                              << std::hex << (int) request.opType ;
                    this->logger->errorLn(&ss);
                    break;
                }
                response.prepareResponse(&request);
                handler->loadResponse(&request, &response);
                response.putData(&this->outputData);
                if (writeAble) {
                    this->logger->infoLn("writeAll.1");
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
            this->logger->infoLn("writeAll.2");
            auto status = this->writeAll(buffer);
            if (status != IoStatus::Ok) {
                return false;
            }
        }
        return true;
    }
}