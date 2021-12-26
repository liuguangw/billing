//
// Created by liuguang on 2021/12/26.
//

#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include "packet_client.h"
#include "../common/billing_exception.h"

namespace services {

    PacketClient::PacketClient(const common::ServerConfig *serverConfig) : serverIp(serverConfig->IP.c_str()),
                                                                           serverPort(serverConfig->Port) {
        if (strcmp("0.0.0.0", this->serverIp) == 0) {
            this->serverIp = "127.0.0.1";
        }
    }

    PacketClient::~PacketClient() {
        if (this->epollFd != -1) {
            close(this->epollFd);
        }
        if (this->connFd != -1) {
            close(this->connFd);
        }
    }

    void PacketClient::sendPacket(const unsigned char *requestBuff, size_t requestBuffSize,
                                  common::BillingPacket *response) {
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
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(this->serverIp);
        serverAddress.sin_port = htons(this->serverPort);
        bool connected = false;
        if (connect(this->connFd, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            if (errno != EINPROGRESS) {
                throw common::BillingException("connect failed", errno);
            }
        } else {
            connected = true;
        }
        this->runLoop(connected, requestBuff, requestBuffSize, response);
    }

    void
    PacketClient::runLoop(bool connected, const unsigned char *requestBuff, size_t requestBuffSize,
                          common::BillingPacket *response) {
        std::vector<unsigned char> responseBuff;
        size_t writeTotalSize = 0;
        auto packetParseResult = common::PacketParseResult::PacketNotFull;
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
                int fd = events[n].data.fd;
                //可写
                if ((events[n].events & EPOLLOUT) != 0) {
                    //判断连接结果
                    if (!connected) {
                        //获取连接的结果
                        int optionErr = 0;
                        socklen_t optionLength = sizeof(optionErr);
                        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &optionErr, &optionLength) < 0) {
                            throw common::BillingException("get socket option failed", errno);
                        }
                        connected = (optionErr == 0);
                        if (!connected) {
                            throw common::BillingException("connect failed", optionErr);
                        }
                    }
                    if (writeTotalSize < requestBuffSize) {
                        this->writeBuff(fd, requestBuff, writeTotalSize,
                                        requestBuffSize - writeTotalSize, &writeTotalSize);
                    }
                }
                //可读
                if ((events[n].events & EPOLLIN) != 0) {
                    this->readPacket(fd, &responseBuff, response, &packetParseResult);
                    if (packetParseResult == common::PacketParseResult::PacketOk) {
                        return;
                    } else if (packetParseResult == common::PacketParseResult::PacketInvalid) {
                        throw common::BillingException("invalid packet", "invalid response packet");
                    }
                }
            }
        }
    }

    void PacketClient::writeBuff(int fd, const unsigned char *buff, size_t offset, size_t leftDataSize,
                                 size_t *writeTotalSize) {
        ssize_t writeSize = write(fd, buff + offset, leftDataSize);
        if (writeSize < 0) {
            if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                throw common::BillingException("write failed", errno);
            }
        } else if (writeSize == 0) {
            throw common::BillingException("remote disconnected", errno);
        } else {
            *writeTotalSize += (size_t) writeSize;
        }
    }

    void
    PacketClient::readPacket(int fd, std::vector<unsigned char> *responseBuff,
                             common::BillingPacket *responsePacket, common::PacketParseResult *packetParseResult) {
        unsigned int buffSize = 1024;
        unsigned char buff[buffSize];
        while (true) {
            ssize_t readCount = read(fd, buff, buffSize);
            if (readCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    break;
                } else {
                    throw common::BillingException("read failed", errno);
                }
            } else if (readCount == 0) {
                throw common::BillingException("remote disconnected", errno);
            }
            //append
            responseBuff->insert(responseBuff->end(), buff, buff + readCount);
            //没有更多数据可以读取了
            if ((unsigned int) readCount < buffSize) {
                break;
            }
        }
        //解析数据包
        *packetParseResult = responsePacket->loadFromSource(responseBuff, 0);
    }

    void PacketClient::waitForConnect(sockaddr *serverAddress, socklen_t serverAddressLength) {
        const int MAX_EVENTS = 5;
        epoll_event events[MAX_EVENTS];
        int fdCount;
        while (true) {
            fdCount = epoll_wait(this->epollFd, events, MAX_EVENTS, -1);
            if (fdCount < 0) {
                throw common::BillingException("epoll wait failed", errno);
            }
            for (int n = 0; n < fdCount; ++n) {
                int fd = events[n].data.fd;
                if (connect(this->connFd, serverAddress, serverAddressLength) < 0) {
                    throw common::BillingException("connect failed", errno);
                }
                return;
            }
        }
    }
}