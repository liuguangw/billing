//
// Created by liuguang on 2021/12/26.
//

#ifndef BILLING_PACKET_CLIENT_H
#define BILLING_PACKET_CLIENT_H

#include <sys/socket.h>
#include "../common/server_config.h"
#include "../common/billing_packet.h"

namespace services {
    class PacketClient {
    private:
        int connFd = -1;
        int epollFd = -1;
        const char *serverIp;
        unsigned short serverPort;

        void runLoop(bool connected, const unsigned char *requestBuff, size_t requestBuffSize,
                     common::BillingPacket *response);

        /**
         * 写buff
         * @param fd
         * @param buff
         * @param offset 偏移位置
         * @param leftDataSize offset到后面剩余的数据长度
         * @param writeTotalSize 写入成功时,增加writeTotalSize的值
         * @return void
         * @throws common::BillingException
         */
        void writeBuff(int fd, const unsigned char *buff, size_t offset, size_t leftDataSize, size_t *writeTotalSize);

        void readPacket(int fd, std::vector<unsigned char> *responseBuff,
                        common::BillingPacket *responsePacket, common::PacketParseResult *packetParseResult);

    public:
        explicit PacketClient(const common::ServerConfig *serverConfig);

        ~PacketClient();

        void sendPacket(const unsigned char *requestBuff, size_t requestBuffSize, common::BillingPacket *response);

        void waitForConnect(sockaddr *serverAddress, socklen_t serverAddressLength);
    };
}
#endif //BILLING_PACKET_CLIENT_H
