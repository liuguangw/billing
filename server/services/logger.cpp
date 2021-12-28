//
// Created by liuguang on 2021/12/20.
//

#include "logger.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
        using std::stringstream;
        using std::endl;
        this->logOutStream.open(logFilePath, ofstream::out | ofstream::app);
        stringstream ss;
        ss << "log file path: " << logFilePath;
        this->infoLn(ss);
    }

    void Logger::infoLn(const char *msg) {
        using std::cout;
        using std::endl;

        cout << colorGreen << "[info]" << colorReset << msg << endl;
        if (this->logOutStream.is_open()) {
            this->logOutStream << "[info]" << msg << endl;
        }
    }

    void Logger::infoLn(const std::stringstream &msg) {
        this->infoLn(msg.str().c_str());
    }

    void Logger::errorLn(const char *msg) {
        using std::cerr;
        using std::endl;

        cerr << colorRed << "[error]" << colorReset << msg << endl;
        if (this->logOutStream.is_open()) {
            this->logOutStream << "[error]" << msg << endl;
        }
    }

    void Logger::errorLn(const std::stringstream &msg) {
        this->errorLn(msg.str().c_str());
    }
}