//
// Created by liuguang on 2021/12/20.
//

#ifndef BILLING_LOGGER_H
#define BILLING_LOGGER_H

#include <fstream>

namespace services {
    using std::ofstream;

    class Logger {
    public:
        ~Logger();

        Logger() = default;

        Logger(const Logger &s) = delete;

        /**
         * @param logFilePath
         * @throws common::BillingException
         */
        void initLogger(const char *logFilePath);

        void infoLn(const char *msg);

        void infoLn(const std::stringstream &msg);

        void errorLn(const char *msg);

        void errorLn(const std::stringstream &msg);

    private:
        ofstream logOutStream;
        const unsigned int timeBufferSize = 29;

        /**
         * 格式化时间
         * @param timeBuffer
         */
        void formatTime(char *timeBuffer) const;
    };
}


#endif //BILLING_LOGGER_H
