//
// Created by liuguang on 2021/12/24.
//

#ifndef BILLING_REGISTER_RESULT_H
#define BILLING_REGISTER_RESULT_H

#include <string>

namespace models {
    struct RegisterResult {
        bool hasError;
        std::string message;
    };
}

#endif //BILLING_REGISTER_RESULT_H
