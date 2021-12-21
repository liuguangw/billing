//
// Created by liuguang on 2021/12/21.
//

#ifndef BILLING_BILLING_EXCEPTION_H
#define BILLING_BILLING_EXCEPTION_H

#include <exception>
#include <string>

namespace common {
    using std::exception;
    using std::string;

    /**
     * 异常定义
     */
    class BillingException : public exception {
    private:
        string message;
    public:
        BillingException(const char *title, const char *message);

        BillingException(const char *title, int errorCode);

        virtual const char *what() const throw() {
            return this->message.c_str();
        }
    };
}


#endif //BILLING_BILLING_EXCEPTION_H
