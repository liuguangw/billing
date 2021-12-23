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
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;

        //尽可能的读取数据,直到缓冲区没有数据可以读取
        common::IoStatus readAll(unsigned char *buff);

        //尽可能的写入数据,直到缓冲区没有空间可以写入
        common::IoStatus writeAll(unsigned char *buff);

    public:
        //读写时buffer的大小
        static const size_t buffSize = 1024;
        explicit TcpConnection(int fd) : connFd(fd) {
            std::cout << "TcpConnection::TcpConnection fd:" << fd << std::endl;
        }

        ~TcpConnection();

        /**
         * 处理数据
         * @param readAble 是否可读
         * @param writeAble 是否可写
         * @return 操作成功时返回
         */
        bool processConn(bool readAble,bool writeAble);
    };
}


#endif //BILLING_TCP_CONNECTION_H
