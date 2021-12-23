//
// Created by liuguang on 2021/12/23.
//
#include <vector>

namespace services{
    template <class T>
    size_t fillBuffer(std::vector<T> *source, size_t offset, T *buff, size_t nBytes){
        size_t fillCount = 0;
        if (source->empty()) {
            return fillCount;
        }
        auto it = source->begin();
        if (offset > 0) {
            it += offset;
        }
        for (size_t i = 0; i < nBytes; i++) {
            if (it == source->end()) {
                break;
            }
            buff[i] = *it;
            fillCount++;
            it++;
        }
        return fillCount;
    }
}