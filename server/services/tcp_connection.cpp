//
// Created by liuguang on 2021/12/21.
//

#include <unistd.h>
#include <iomanip>
#include <cstring>
#include "../common/billing_exception.h"
#include "../bhandler/handlers.h"
#include "../debug/dump_buffer.h"
#include "../debug/dump_packet.h"
#include "tcp_connection.h"
#include "io_tools.h"

namespace services {

    TcpConnection::TcpConnection(int fd, const char *ipAddress, unsigned short port, HandlerResource &hResource)
            : connFd(fd), ipAddress(ipAddress), port(port), handlerResource(hResource) {
        this->initPacketHandlers();
        auto &logger = this->handlerResource.logger();
        std::stringstream ss;
        ss << "TcpConnection::TcpConnection fd:" << fd;
        logger.infoLn(ss);
        ss.str("");
        ss << "client " << ipAddress << ":" << port << " connected";
        logger.infoLn(ss);
    }

    TcpConnection::~TcpConnection() {
        auto &logger = this->handlerResource.logger();
        logger.infoLn("TcpConnection::~TcpConnection");
        for (auto &pair: this->packetHandlers) {
            delete pair.second;
        }
        close(this->connFd);
        std::stringstream ss;
        ss << "TcpConnection destroy fd:" << this->connFd;
        ss << "(" << this->ipAddress << ":" << this->port << ")";
        logger.infoLn(ss);
    }


    void TcpConnection::addHandler(common::PacketHandler *handler) {
        this->packetHandlers[handler->getType()] = handler;
    }

    void TcpConnection::initPacketHandlers() {
        common::PacketHandler *handler = new bhandler::CommandHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::ConnectHandler;
        this->addHandler(handler);
        handler = new bhandler::PingHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::LoginHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::EnterGameHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::LogoutHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::KeepHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::KickHandler;
        this->addHandler(handler);
        handler = new bhandler::CostLogHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::ConvertPointHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::QueryPointHandler(this->handlerResource);
        this->addHandler(handler);
        handler = new bhandler::RegisterHandler(this->handlerResource);
        this->addHandler(handler);
    }

    bool TcpConnection::processConn(bool readAble, bool writeAble) {
        ssize_t tmpSize;
        auto &logger = this->handlerResource.logger();
        //写入上一轮没有成功写入的数据
        if (writeAble && !this->outputData.empty()) {
            if (!this->processSendOutputData()) {
                return false;
            }
            //系统缓冲区满了,暂时不能写入更多数据了
            if (!this->outputData.empty()) {
                writeAble = false;
            }
        }
        if (readAble) {
            const unsigned int buffSize = 1024;
            unsigned char buffer[buffSize];
            //logger.infoLn("readAll");
            tmpSize = ioReadAll(this->connFd, this->inputData, buffer, buffSize);
            if (tmpSize < 0) {
                std::stringstream ss;
                ss << "read failed: " << strerror(errno);
                logger.errorLn(ss);
                return false;
            } else if (tmpSize == 0) {
                logger.infoLn("client disconnected");
                return false;
            }
        }
        //处理读取到的数据
        if (!this->inputData.empty()) {
            return this->processInputData(writeAble);
        }
        return true;
    }


    bool TcpConnection::processInputData(bool writeAble) {
        using common::PacketParseResult;
        common::BillingPacket request, response;
        //已成功解析的数据包总大小
        size_t parseTotalSize = 0;
        PacketParseResult parseResult;
        std::map<unsigned char, common::PacketHandler *>::iterator handlerIt;
        common::PacketHandler *handler;
        auto &logger = this->handlerResource.logger();
        while (true) {
            parseResult = request.loadFromSource(&this->inputData, parseTotalSize);
            if (parseResult == PacketParseResult::PacketNotFull) {
                //数据包结构不完整,跳出while解析循环
                break;
            }
            //格式错误
            if (parseResult == PacketParseResult::PacketInvalid) {
                logger.errorLn("invalid packet");
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
                logger.errorLn(ss);
                ss.str("");
                debug::dumpPacket(ss, "request", &request);
                logger.infoLn(ss);
                continue;
            }
            response.prepareResponse(request);
            handler->loadResponse(request, response);
            response.packData(this->outputData);
            if (writeAble) {
                if (!this->processSendOutputData()) {
                    return false;
                }
                //系统缓冲区满了,暂时不能写入更多数据了
                if (!this->outputData.empty()) {
                    writeAble = false;
                }
            }
        }
        if (parseTotalSize > 0) {
            //删除左侧已经解析了的数据
            ioRemoveLeftBuffer(this->inputData, parseTotalSize);
        }
        return true;
    }

    bool TcpConnection::processSendOutputData() {
        ssize_t writeCount = ioWriteBuffer(this->connFd, this->outputData.data(), this->outputData.size());
        if (writeCount < 0) {
            std::stringstream ss;
            ss << "write failed: " << strerror(errno);
            this->handlerResource.logger().errorLn(ss);
            return false;
        } else if (writeCount > 0) {
            //删除左侧已经发送了的数据
            ioRemoveLeftBuffer(this->outputData, (size_t) writeCount);
        }
        return true;
    }
}
