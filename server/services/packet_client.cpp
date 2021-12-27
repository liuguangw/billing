//
// Created by liuguang on 2021/12/26.
//

#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <vector>
#include "packet_client.h"
#include "../common/billing_exception.h"
#include "io_tools.h"

namespace services {
    PacketClient::PacketClient(const common::ServerConfig &serverConfig) {
        //设置地址,端口
        this->serverEndpoint.sin_family = AF_INET;
        if (serverConfig.IP == "0.0.0.0") {
            this->serverEndpoint.sin_addr.s_addr = inet_addr("127.0.0.1");
        } else {
            this->serverEndpoint.sin_addr.s_addr = inet_addr(serverConfig.IP.c_str());
        }
        this->serverEndpoint.sin_port = htons(serverConfig.Port);
    }

    PacketClient::~PacketClient() {
        if (this->epollFd != -1) {
            close(this->epollFd);
        }
        if (this->connFd != -1) {
            close(this->connFd);
        }
        //std::cout<<"PacketClient::~PacketClient"<<std::endl;
    }

    bool PacketClient::initSocket() {
        this->epollFd = epoll_create1(0);
        if (this->epollFd == -1) {
            throw common::BillingException("init epoll fd failed", errno);
        }
        this->connFd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (this->connFd < 0) {
            throw common::BillingException("init socket failed", errno);
        }
        int option = 1;
        setsockopt(this->connFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        //注册事件
        epoll_event epollEvent{};
        epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
        epollEvent.data.fd = this->connFd;
        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, this->connFd, &epollEvent) == -1) {
            throw common::BillingException("epoll register connection socket failed", errno);
        }
        //connect
        bool connected = false;
        if (connect(this->connFd, (sockaddr *) &this->serverEndpoint, sizeof(this->serverEndpoint)) < 0) {
            if (errno != EINPROGRESS) {
                throw common::BillingException("connect failed", errno);
            }
        } else {
            connected = true;
        }
        return connected;
    }

    common::BillingPacket PacketClient::execute(const common::BillingPacket &requestPacket) {
        bool connected = this->initSocket();
        common::BillingPacket responsePacket;
        this->runLoop(connected, requestPacket, responsePacket);
        return responsePacket;
    }

    void PacketClient::checkConnectionStatus() const {
        //获取连接的结果
        int optionErr = 0;
        socklen_t optionLength = sizeof(optionErr);
        if (getsockopt(this->connFd, SOL_SOCKET, SO_ERROR, &optionErr, &optionLength) < 0) {
            throw common::BillingException("get socket option failed", errno);
        }
        if (optionErr != 0) {
            throw common::BillingException("connect failed", optionErr);
        }
    }

    void
    PacketClient::runLoop(bool connected, const common::BillingPacket &requestPacket,
                          common::BillingPacket &responsePacket) {
        size_t writeTotalSize = 0;
        std::vector<unsigned char> requestData, responseData;
        requestPacket.packData(requestData);
        size_t requestDataSize = requestPacket.fullLength();
        //
        const unsigned int tmpBufferSize = 1024;
        unsigned char tmpBuffer[tmpBufferSize];
        ssize_t tmpSize;
        //
        const int MAX_EVENTS = 5;
        epoll_event events[MAX_EVENTS];
        int fdCount;
        while (true) {
            fdCount = epoll_wait(this->epollFd, events, MAX_EVENTS, -1);
            if (fdCount < 0) {
                throw common::BillingException("epoll wait failed", errno);
            }
            for (int n = 0; n < fdCount; ++n) {
                //可写
                if ((events[n].events & EPOLLOUT) != 0) {
                    //判断连接结果
                    if (!connected) {
                        this->checkConnectionStatus();
                        connected = true;
                    }
                    if (writeTotalSize < requestDataSize) {
                        tmpSize = ioWriteBuffer(this->connFd, requestData.data(), requestDataSize,
                                                writeTotalSize);
                        if (tmpSize < 0) {
                            throw common::BillingException("write failed", errno);
                        }
                        writeTotalSize += (size_t) tmpSize;
                    }
                }
                //可读
                if ((events[n].events & EPOLLIN) != 0) {
                    tmpSize = ioReadAll(this->connFd, responseData, tmpBuffer, tmpBufferSize);
                    if (tmpSize == 0) {
                        throw common::BillingException("read failed", "remote disconnected");
                    } else if (tmpSize < 0) {
                        throw common::BillingException("read failed", errno);
                    }
                    auto packetParseResult = responsePacket.loadFromSource(&responseData, 0);
                    if (packetParseResult == common::PacketParseResult::PacketOk) {
                        return;
                    } else if (packetParseResult == common::PacketParseResult::PacketInvalid) {
                        throw common::BillingException("invalid packet", "invalid response packet");
                    }
                }
            }
        }
    }
}