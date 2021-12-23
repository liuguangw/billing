//
// Created by liuguang on 2021/12/20.
//

#ifndef BILLING_LOGGER_H
#define BILLING_LOGGER_H

#include <fstream>

namespace services {
    using std::ofstream;
    class Logger {
    private:
        ofstream logOutStream;
    public:
        ~Logger();

        void initLogger(const char *logFilePath);

        void infoLn(const char *msg);
        void infoLn(const std::stringstream* msg);

        void errorLn(const char *msg);
        void errorLn(const std::stringstream* msg);
    };
}


#endif //BILLING_LOGGER_H
