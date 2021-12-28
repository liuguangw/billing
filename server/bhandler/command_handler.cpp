//
// Created by liuguang on 2021/12/27.
//

#include "command_handler.h"
#include "../services/packet_data_reader.h"

namespace bhandler {
    using std::string;
    using services::PacketDataReader;

    void CommandHandler::loadResponse(const BillingPacket &request, BillingPacket &response) {
        string command = PacketDataReader::buildString(request.opData);
        auto &logger = this->handlerResource.logger();
        //std::cout << "command=[" << command << "]" << std::endl;
        if (command == "show_users") {
            std::stringstream ss;
            this->dumpUsers(ss);
            //std::cout << ss.str().c_str() << std::endl;
            response.appendOpData(ss.str());
        } else {
            //stop
            if (command == "stop") {
                this->handlerResource.markStop(true);
            }
            response.appendOpData(common::PACKET_RESULT_SUCCESS);
        }
    }

    void CommandHandler::dumpUsers(std::stringstream &ss) {
        ss << "login users:";
        auto &loginUsers = this->handlerResource.loginUsers();
        if (loginUsers.empty()) {
            ss << " empty";
        } else {
            for (auto &pairs: loginUsers) {
                ss << "\n\t" << pairs.first << ": " << pairs.second;
            }
        }
        //
        ss << "\n\nonline users:";
        auto &onlineUsers = this->handlerResource.onlineUsers();
        if (onlineUsers.empty()) {
            ss << " empty";
        } else {
            for (auto &pairs: onlineUsers) {
                ss << "\n\t" << pairs.first << ": " << pairs.second;
            }
        }
        //
        ss << "\n\nmac counters:";
        auto &macCounters = this->handlerResource.macCounters();
        if (macCounters.empty()) {
            ss << " empty";
        } else {
            for (auto &pairs: macCounters) {
                ss << "\n\t" << pairs.first << ": " << pairs.second;
            }
        }
    }
}