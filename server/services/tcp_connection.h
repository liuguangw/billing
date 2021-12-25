//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_TCP_CONNECTION_H
#define BILLING_TCP_CONNECTION_H


#include <map>
#include <vector>
#include "../common/io_status.h"
#include "../common/packet_handler.h"
#include "logger.h"
#include "handler_resource.h"

namespace services {
    class TcpConnection {
    private:
        int connFd;
        std::string ipAddress;
        unsigned short port;
        HandlerResource *handlerResource;
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;
        std::map<unsigned char, common::PacketHandler *> packetHandlers;

        //尽可能的读取数据,直到缓冲区没有数据可以读取
        common::IoStatus readAll(unsigned char *buff);

        //尽可能的写入数据,直到缓冲区没有空间可以写入
        common::IoStatus writeAll(unsigned char *buff);

        void addHandler(common::PacketHandler *handler);

        //初始化handlers
        void initPacketHandlers();

    public:
        //读写时buffer的大小
        static const size_t buffSize = 1024;

        explicit TcpConnection(int fd, const char *ipAddress, unsigned short port, HandlerResource *hResource);

        ~TcpConnection();

        /**
         * 处理连接事件
         * @param readAble 是否可读
         * @param writeAble 是否可写
         * @return 操作成功时返回
         */
        bool processConn(bool readAble, bool writeAble);

        /**
         * 处理读取到的数据
         * @param writeAble 是否可写
         * @param buffer 用于写入的临时buffer
         * @return 操作成功时返回
         */
        bool processInputData(bool writeAble, unsigned char *buffer);
    };
}


#endif //BILLING_TCP_CONNECTION_H
