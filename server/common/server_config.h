//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_SERVER_CONFIG_H
#define BILLING_SERVER_CONFIG_H

#include <string>
#include <vector>

namespace common {
    using std::string;
    using std::vector;

    class ServerConfig {
    public:
        string IP;   //billing服务器listen的IP
        unsigned short Port;      //billing服务器listen的端口
        string DbHost;              //数据库主机名或者IP
        unsigned int DbPort;                        //数据库端口
        string DbUser;                     //数据库用户名
        string DbPassword;                 //数据库密码
        string DbName;                         //数据库名
        bool AllowOldPassword,    //是否启用oldPassword(除非报这个错误,否则不建议开启)
        AutoReg;                     //是否开启自动注册
        vector<string> AllowIps;               //允许连接billing的服务端IP,为空则表示不限制
        unsigned int MaxClientCount,            //最多允许进入的用户数量(0表示无限制)
        PcMaxClientCount;      //每台电脑最多允许进入的用户数量(0表示无限制)
        ServerConfig();

        void initConfig(const char *configFilePath);
    };
}
#endif //BILLING_SERVER_CONFIG_H
