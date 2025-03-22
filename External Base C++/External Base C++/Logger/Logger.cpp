#include "../include.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    logfile_.open(Globals::GetWorkingPath().string() + "\\logs.txt", std::ios::trunc);
    if (!logfile_) {
        std::cerr << "Failed to open log file!" << std::endl;
    }
}

Logger::~Logger() {
    if (logfile_.is_open()) {
        logfile_.close();
    }
}

std::string Logger::getLogLevelString(LogLevel level) {
    switch (level) {
    case LogLevel::LOG_INFO: return "INFO";
    case LogLevel::LOG_WARNING: return "WARNING";
    case LogLevel::LOG_ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}