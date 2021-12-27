//
// Created by liuguang on 2021/12/27.
//

#ifndef BILLING_IO_TOOLS_H
#define BILLING_IO_TOOLS_H

#include <vector>
#include "../common/billing_exception.h"

namespace services {
    /**
     * 从fd中尽可能的读取数据,直到缓冲区暂时没有数据可读,读取到的数据会append到data后面
     * @tparam T
     * @param fd
     * @param data
     * @param buffer
     * @param bufferSize
     * @return 本次读取到的数据总大小,如果为0表示end of file或者连接断开,-1则表示读取错误(用errno进行判断)
     */
    template<class T>
    ssize_t ioReadAll(int fd, std::vector<T> &data, T buffer[], size_t bufferSize) {
        ssize_t readTotalCount = 0, readCount;
        while (true) {
            readCount = read(fd, buffer, bufferSize);
            if (readCount < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    break;
                }
                return -1;
            } else if (readCount == 0) {
                return 0;
            }
            readTotalCount += readCount;
            //append
            data.insert(data.end(), buffer, buffer + (size_t) readCount);
            //没有更多数据可以读取了
            if ((size_t) readCount < bufferSize) {
                break;
            }
        }
        return readTotalCount;
    }

    /**
     * 写入数据
     * @tparam T
     * @param fd
     * @param buffer 要写入的buffer
     * @param bufferTotalSize buffer的总大小
     * @param offset 从哪里开始写
     * @return 本次写入的数据总大小,-1则表示写入错误(用errno进行判断)
     */
    template<class T>
    ssize_t ioWriteBuffer(int fd, const T *buffer, size_t bufferTotalSize, size_t offset = 0) {
        ssize_t writeSize = write(fd, buffer + offset, bufferTotalSize - offset);
        if (writeSize < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                //阻塞了
                return 0;
            } else {
                return -1;
            }
        }
        return writeSize;
    }

    /**
     * 删除左侧一定长度的数据
     * @tparam T
     * @param data
     * @param removeCount 左侧要删除的数据长度
     */
    template<class T>
    void ioRemoveLeftBuffer(std::vector<T> &data, size_t removeCount) {
        //直接清空
        if (removeCount >= data.size()) {
            data.clear();
            return;
        }
        auto begin = data.begin();
        data.erase(begin, begin + removeCount);
    }
}
#endif //BILLING_IO_TOOLS_H
