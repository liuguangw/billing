//
// Created by liuguang on 2021/12/20.
//

#include "logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include "../common/billing_exception.h"

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

        this->logOutStream.exceptions(ofstream::failbit);
        try {
            this->logOutStream.open(logFilePath, ofstream::out | ofstream::app);
        } catch (ofstream::failure &ex) {
            throw common::BillingException("open log file failed", ex.what());
        }
        std::stringstream ss;
        ss << "log file path: " << logFilePath;
        this->infoLn(ss);
    }

    void Logger::infoLn(const char *msg) {
        using std::cout;
        using std::endl;
        char timeBuffer[this->timeBufferSize];
        this->formatTime(timeBuffer);

        cout << "[" << timeBuffer << "]" << colorGreen << "[info]  " << colorReset << msg << endl;
        if (this->logOutStream.is_open()) {
            this->logOutStream << "[" << timeBuffer << "][info]  " << msg << endl;
        }
    }

    void Logger::infoLn(const std::stringstream &msg) {
        this->infoLn(msg.str().c_str());
    }

    void Logger::errorLn(const char *msg) {
        using std::cerr;
        using std::endl;
        char timeBuffer[this->timeBufferSize];
        this->formatTime(timeBuffer);

        cerr << "[" << timeBuffer << "]" << colorRed << "[error] " << colorReset << msg << endl;
        if (this->logOutStream.is_open()) {
            this->logOutStream << "[" << timeBuffer << "][error] " << msg << endl;
        }
    }

    void Logger::errorLn(const std::stringstream &msg) {
        this->errorLn(msg.str().c_str());
    }

    void Logger::formatTime(char *timeBuffer) const {
        auto currentTime = time(nullptr);
        auto localTime = localtime(&currentTime);
        strftime(timeBuffer, this->timeBufferSize, "%F %T GMT%z", localTime);
    }
}