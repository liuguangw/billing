//
// Created by liuguang on 2021/12/21.
//

#include <unistd.h>
#include <iomanip>
#include "../common/billing_exception.h"
#include "../bhandler/connect_handler.h"
#include "../bhandler/ping_handler.h"
#include "../bhandler/login_handler.h"
#include "../bhandler/enter_game_handler.h"
#include "../bhandler/logout_handler.h"
#include "../debug/dump_buffer.h"
#include "../debug/dump_packet.h"
#include "tcp_connection.h"
#include "fill_buffer.h"

namespace services {
    using common::IoStatus;

    TcpConnection::TcpConnection(int fd, const char *ipAddress, unsigned short port, HandlerResource *hResource)
            : connFd(fd), ipAddress(ipAddress), port(port), handlerResource(hResource) {
        this->initPacketHandlers();
        auto logger = this->handlerResource->logger();
        std::stringstream ss;
        ss << "TcpConnection::TcpConnection fd:" << fd;
        logger->infoLn(&ss);
        ss.str("");
        ss << "client " << ipAddress << ":" << port << " connected";
        logger->infoLn(&ss);
    }

    TcpConnection::~TcpConnection() {
        auto logger = this->handlerResource->logger();
        logger->infoLn("TcpConnection::~TcpConnection");
        for (auto &pair: this->packetHandlers) {
            delete pair.second;
        }
        close(this->connFd);
        std::stringstream ss;
        ss << "TcpConnection destroy fd:" << this->connFd;
        ss << "(" << this->ipAddress << ":" << this->port << ")";
        logger->infoLn(&ss);
    }

    IoStatus TcpConnection::readAll(unsigned char *buff) {
        ssize_t readCount;
        while (true) {
            readCount = read(this->connFd, buff, buffSize);
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
            //缓冲区没有数据可以读取了
            if ((size_t) readCount < buffSize) {
                return IoStatus::Pending;
            }
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
            //缓冲区满了
            if ((size_t) writeCount < fillCount) {
                result = IoStatus::Pending;
                break;
            }
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
        handler = new bhandler::PingHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::LoginHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::EnterGameHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::LogoutHandler(this->handlerResource);
        this->addHandler(handler);
    }

    bool TcpConnection::processConn(bool readAble, bool writeAble) {
        unsigned char buffer[buffSize];
        //写入上一轮没有成功写入的数据
        if (writeAble && !this->outputData.empty()) {
            //logger->infoLn("writeAll");
            auto status = this->writeAll(buffer);
            if ((status == IoStatus::Error) || (status == IoStatus::Disconnected)) {
                return false;
            }
        }
        if (readAble) {
            //logger->infoLn("readAll");
            auto status = this->readAll(buffer);
            if (status != IoStatus::Pending) {
                return false;
            }
        }
        //处理读取到的数据
        if (!this->inputData.empty()) {
            return this->processInputData(writeAble, buffer);
        }
        return true;
    }


    bool TcpConnection::processInputData(bool writeAble, unsigned char *buffer) {
        using common::PacketParseResult;

        common::BillingPacket request, response;
        PacketParseResult parseResult;
        //已成功解析的数据包总大小
        size_t parseTotalSize = 0;
        std::map<unsigned char, common::PacketHandler *>::iterator handlerIt;
        common::PacketHandler *handler;
        auto logger = this->handlerResource->logger();
        while (true) {
            parseResult = request.loadFromSource(&this->inputData, parseTotalSize);
            if (parseResult == PacketParseResult::PacketNotFull) {
                //数据包结构不完整,跳出while解析循环
                break;
            }
            //格式错误
            if (parseResult == PacketParseResult::PacketInvalid) {
                logger->errorLn("invalid packet");
                return false;
            }
            parseTotalSize += request.fullLength();
            //根据类型,查找handler
            handlerIt = this->packetHandlers.find(request.opType);
            if (handlerIt != this->packetHandlers.end()) {
                handler = handlerIt->second;
            } else {
                //记录无法处理的packet
                std::stringstream ss;
                ss << "unknown packet opType: 0x" << std::setfill('0') << std::setw(2) << std::right
                   << std::hex << (int) request.opType;
                logger->errorLn(&ss);
                ss.str("");
                debug::dumpPacket(ss, "request", &request);
                logger->infoLn(&ss);
                break;
            }
            response.prepareResponse(&request);
            handler->loadResponse(&request, &response);
            response.appendToOutput(&this->outputData);
            if (writeAble) {
                //logger->infoLn("writePacket");
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
                this->inputData.erase(it, it + (int) parseTotalSize);
            } else {
                //全部解析完了,直接清空
                this->inputData.clear();
            }
        }
        return true;
    }
}
