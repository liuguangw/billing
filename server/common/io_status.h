//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_IO_STATUS_H
#define BILLING_IO_STATUS_H

namespace common {
    enum IoStatus{
        //成功
        Ok,
        //失败
        Error,
        //暂时无法读取或者写入
        Pending,
        //连接断开了
        Disconnected
 };
}
#endif //BILLING_IO_STATUS_H
