//
// Created by liuguang on 2021/12/20.
//

#include "logger.h"
#include <iostream>
#include <fstream>

namespace services {
    const char *colorRed = "\033[31m";
    const char *colorGreen = "\033[32m";
    const char *colorReset = "\033[0m";

    Logger::~Logger() {
        if (this->logOutStream.is_open()) {
            this->logOutStream.close();
        }
    }

    void Logger::initLogger(const char *logFilePath) {
        using std::ofstream;
        using std::cout;
        using std::endl;
        cout << "log file path: " << logFilePath << endl;
        this->logOutStream.open(logFilePath, ofstream::out | ofstream::app);
    }

    void Logger::infoLn(const char *msg) {
        using std::cout;
        using std::endl;

        cout << colorGreen << "[info]" << colorReset << msg << endl;
        this->logOutStream << "[info]" << msg << endl;
    }

    void Logger::errorLn(const char *msg) {
        using std::cerr;
        using std::endl;

        cerr << colorRed << "[error]" << colorReset << msg << endl;
        this->logOutStream << "[error]" << msg << endl;
    }
}