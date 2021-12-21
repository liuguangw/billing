//
// Created by liuguang on 2021/12/21.
//

#include <cstring>
#include "billing_exception.h"

namespace common {
    BillingException::BillingException(const char *title, const char *message) {
        this->message.append(title).append(": ").append(message);
    }

    BillingException::BillingException(const char *title, int errorCode)  {
        this->message.append(title).append(": ").append(strerror(errorCode));
    }
}