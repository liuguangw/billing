//
// Created by liuguang on 2021/12/21.
//

#include <cstring>
#include "billing_exception.h"

namespace common {
    BillingException::BillingException(const char *title, const char *message) : exception() {
        this->message.append(title).append(": ").append(message);
    }

    BillingException::BillingException(const char *title, int errorCode) : exception() {
        this->message.append(title).append(": ").append(strerror(errorCode));
    }
}