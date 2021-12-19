//
// Created by liuguang on 2021/12/19.
//

#ifndef BILLING_DATABASE_CONNECTION_H
#define BILLING_DATABASE_CONNECTION_H

#include <mysql.h>

namespace services {
    class DatabaseConnection {
    private:
        const char *host;
        const char *user;
        const char *passwd;
        const char *db;
        unsigned int port;
        MYSQL *mysql;
    public:
        /**
         * 构造函数
         * @param host 数据库服务器主机名称或者ip
         * @param user 数据库用户名
         * @param passwd 数据库密码
         * @param db 数据库名称
         * @param port 端口
         */
        DatabaseConnection(const char *host,
                           const char *user,
                           const char *passwd,
                           const char *db,
                           unsigned int port) : host(host), user(user), passwd(passwd), db(db), port(port) {
            mysql = mysql_init(NULL);
        }

        /**
         * 析构函数
         */
        ~DatabaseConnection();

        /**
         * 连接数据库
         * @return 连接是否成功
         */
        bool connect();

        /**
         * 获取最后一次的错误信息
         * @return
         */
        const char *lastError();

        /**
         * 获取服务器版本信息
         * @return
         */
        const char *serverVersion();
    };
}


#endif //BILLING_DATABASE_CONNECTION_H
