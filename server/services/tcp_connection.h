//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_TCP_CONNECTION_H
#define BILLING_TCP_CONNECTION_H

#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include "../common/io_status.h"
#include <pthread.h>

namespace services {

    void *processTcpData(void *arg);

    class TcpConnection {
    private:
        int connFd;
        bool writeAble = false;
        pthread_t threadT = 0;
        pthread_mutex_t inputDataMutex{};
        pthread_mutex_t writeAbleMutex{};
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;

        static size_t fillBuffer(std::vector<unsigned char> *source, size_t offset, unsigned char *buff, size_t nBytes);

        void processData();

        //尽可能的写入数据,直到缓冲区没有空间可以写入
        common::IoStatus writeAll(unsigned char *buff, size_t nBytes);

    public:
        bool needStop = false;

        explicit TcpConnection(int fd) : connFd(fd){
            pthread_mutex_init(&this->inputDataMutex, nullptr);
            pthread_mutex_init(&this->writeAbleMutex, nullptr);
            std::cout << "TcpConnection construct fd:" << fd << std::endl;
        }

        ~TcpConnection();

        void setWriteAble(bool s) {
            this->writeAble = s;
        }

        void startWorkingThread();

        int lock(bool isInput) {
            if (isInput) {
                return pthread_mutex_lock(&this->inputDataMutex);
            } else {
                return pthread_mutex_lock(&this->writeAbleMutex);
            }
        }

        int unlock(bool isInput) {
            if (isInput) {
                return pthread_mutex_unlock(&this->inputDataMutex);
            } else {
                return pthread_mutex_unlock(&this->writeAbleMutex);
            }
        }

        //尽可能的读取数据,直到缓冲区没有数据可以读取
        common::IoStatus readAll(unsigned char *buff, size_t nBytes);

        //友元函数,用于执行子线程逻辑
        friend void *processTcpData(void *arg);
    };
}


#endif //BILLING_TCP_CONNECTION_H
