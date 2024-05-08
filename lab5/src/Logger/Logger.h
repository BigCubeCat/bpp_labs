#ifndef LAB5_LOGGER_H
#define LAB5_LOGGER_H

#include <string>

enum LOG_LEVEL {
    DEBUG,
    INFO,
    WARN,
    ERROR
};


class Logger {
private:
    LOG_LEVEL logLevel;
    std::string rank;

    std::string getLogString(const std::string &logMessage);

    static std::string stringifyLevel(const LOG_LEVEL &lvl);

public:
    Logger(int mpiRank, LOG_LEVEL level);

    void debug(const std::string &logMessage);

    void info(const std::string &logMessage);

    void warn(const std::string &logMessage);

    void err(const std::string &logMessage);
};


#endif
