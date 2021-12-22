//
// Created by liuguang on 2021/12/21.
//

#include "tcp_connection.h"
#include "../common/billing_exception.h"
#include "../common/billing_packet.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <csignal>

namespace services {
    using common::IoStatus;

    TcpConnection::~TcpConnection() {
        std::cout << "call TcpConnection::~TcpConnection"  << std::endl;
        close(this->connFd);
        if (this->threadT != 0) {
            pthread_cancel(this->threadT);
            std::cout << "stop child thread fd:" << this->connFd << std::endl;
        }
        pthread_mutex_destroy(&this->inputDataMutex);
        pthread_mutex_destroy(&this->writeAbleMutex);
        std::cout << "TcpConnection destroy fd:" << this->connFd << std::endl;
    }

    size_t
    TcpConnection::fillBuffer(std::vector<unsigned char> *source, size_t offset, unsigned char *buff, size_t nBytes) {
        size_t fillCount = 0;
        if (source->empty()) {
            return fillCount;
        }
        auto it = source->begin();
        if (offset > 0) {
            it += offset;
        }
        for (size_t i = 0; i < nBytes; i++) {
            if (it == source->end()) {
                break;
            }
            buff[i] = *it;
            fillCount++;
            it++;
        }
        return fillCount;
    }

    IoStatus TcpConnection::readAll(unsigned char *buff, size_t nBytes) {
        ssize_t readCount;
        while (true) {
            readCount = read(this->connFd, buff, nBytes);
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
            std::cout << "====================================================" << std::endl;
            for (std::size_t i = 0; i < this->inputData.size(); i++) {
                std::cout << std::hex << (int) this->inputData[i] << " ";
                if ((i % 16 == 15) || (i == this->inputData.size() - 1)) {
                    std::cout << std::endl;
                }
            }
            std::cout << "====================================================" << std::endl;
        }
    }

    common::IoStatus TcpConnection::writeAll(unsigned char *buff, size_t nBytes) {
        size_t writeCountTotal = 0, writeCount, fillCount;
        while (writeCountTotal < this->outputData.size()) {
            fillCount = fillBuffer(&this->outputData, writeCountTotal, buff, nBytes);
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

    //启动工作线程
    void TcpConnection::startWorkingThread() {
        pthread_attr_t attr;
        if (pthread_attr_init(&attr) != 0) {
            throw common::BillingException("init pthread attr failed", errno);
        }
        if (pthread_create(&this->threadT, &attr, &processTcpData, this) != 0) {
            throw common::BillingException("create thread failed", errno);
        }
    }

    void TcpConnection::processData() {
        std::cout << "[" << this->threadT << "]hello this is child !" << std::endl;
        common::BillingPacket packet;
        int loadStatus;
        while (true) {
            if (this->needStop) {
                break;
            }
            this->lock(true);
            loadStatus = packet.load(&this->inputData);
            if (loadStatus != 0) {
                this->unlock(true);
            }
            //std::cout << "packet load status: " << loadStatus << std::endl;
            if (loadStatus == 0) {
                //删除左侧
                auto pos = this->inputData.begin();
                this->inputData.erase(pos, pos + packet.fullLength());
                this->unlock(true);
                //todo response
            } else if (loadStatus == 1) {
                usleep(500000);
                continue;
            } else {
                //数据包格式错误的处理
                std::cout << "packet error fd:" <<this->connFd<< std::endl;
                close(this->connFd);
                this->connFd =-1;
                return;
            }
        }
    }

    void *processTcpData(void *arg) {
        auto tcpConn = static_cast<TcpConnection *>(arg);
        tcpConn->processData();
        for (int i = 0; i < 5; i++) {
            std::cout << std::dec << "...." << i << std::endl;
            sleep(1);
        }
        return nullptr;
    }
}