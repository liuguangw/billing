//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_TCP_CONNECTION_H
#define BILLING_TCP_CONNECTION_H


#include <map>
#include <vector>
#include "../common/packet_handler.h"
#include "logger.h"
#include "handler_resource.h"

namespace services {
    class TcpConnection {
    public:

        explicit TcpConnection(int fd, const char *ipAddress, unsigned short port, HandlerResource &hResource);

        ~TcpConnection();

        /**
         * 处理连接事件
         * @param readAble 是否可读
         * @param writeAble 是否可写
         * @return 操作成功时返回true
         */
        bool processConn(bool readAble, bool writeAble);

    private:
        // socket连接
        int connFd;
        std::string ipAddress;
        unsigned short port;
        HandlerResource &handlerResource;
        std::vector<unsigned char> inputData;
        std::vector<unsigned char> outputData;
        std::map<unsigned char, common::PacketHandler *> packetHandlers;

        /**
         * 处理读取到的数据
         * @param writeAble 是否可写
         * @return 操作成功时返回true
         */
        bool processInputData(bool writeAble);

        /**
         * 处理发送outputData里的数据
         * @return 操作成功时返回true
         */
        bool processSendOutputData();

        //添加handler
        void addHandler(common::PacketHandler *handler);

        //初始化handlers
        void initPacketHandlers();
    };
}


#endif //BILLING_TCP_CONNECTION_H
