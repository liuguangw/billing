//
// Created by liuguang on 2021/12/26.
//

#ifndef BILLING_PACKET_CLIENT_H
#define BILLING_PACKET_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "../common/server_config.h"
#include "../common/billing_packet.h"

namespace services {
    class PacketClient {
    public:
        explicit PacketClient(const common::ServerConfig &serverConfig);

        ~PacketClient();
        /**
         * 执行发送packet
         * @return 返回第一个响应包
         * @throws common::BillingException
         */
        common::BillingPacket execute(const common::BillingPacket &requestPacket);

    private:
        int connFd = -1;
        int epollFd = -1;
        //服务器的地址+端口信息
        sockaddr_in serverEndpoint{};

        /**
         * 初始化socket连接
         * @return bool 连接是否已经成功建立
         * @throws common::BillingException
         */
        bool initSocket();

        /**
         * 运行epoll循环,直到读取到响应包
         * @param connected
         * @param requestPacket
         * @param responsePacket
         * @throws common::BillingException
         */
        void runLoop(bool connected,const common::BillingPacket &requestPacket, common::BillingPacket& responsePacket);

        /**
         * 检测connect的结果,如果结果是失败则抛出异常
         * @throws common::BillingException
         */
        void checkConnectionStatus() const;
    };
}
#endif //BILLING_PACKET_CLIENT_H
