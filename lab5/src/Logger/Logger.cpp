#include <iostream>
#include "Logger.h"


std::string Logger::getLogString(const std::string &logMessage) {
    return "rank" + rank + "\t" + logMessage + "\n";
}

Logger::Logger(int mpiRank, LOG_LEVEL level) : logLevel(level) {
    rank = std::to_string(mpiRank);
}

void Logger::debug(const std::string &logMessage) {
    if (logLevel == DEBUG)
        std::cout << "[DEBUG] " << getLogString(logMessage);
}

void Logger::info(const std::string &logMessage) {
    if (logLevel <= INFO)
        std::cout << getLogString(logMessage);
}

void Logger::warn(const std::string &logMessage) {
    if (logLevel <= WARN)
        std::cout << "[WARN] " << getLogString(logMessage);
}

void Logger::err(const std::string &logMessage) {
    std::cerr << getLogString(logMessage);
}

std::string Logger::stringifyLevel(const LOG_LEVEL &lvl) {
    switch (lvl) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        default:
            return "ERR";
    }
}
