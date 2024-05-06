#include "Config.h"

#include <iostream>

Config::Config(int argc, char **argv) {
    std::string argument = std::string(argv[1]);
    if (argc == 2) {
        if (argument == "env") {
            usingEnv = true;
            parseEnv();
            return;
        } else {
            if (argument == "balance=true") {
                useBalance = true;
            } else if (argument == "balance=false") {
                useBalance = false;
            }
        }
    }
}

void Config::parseEnv() {
    defaultCountTasks = parseEnvInt((char *) "COUNT_TASKS");
    useBalance = parseEnvBool((char *) "BALANCE");
    syncDelay = parseEnvInt((char *) "SYNC");
    storeSize = parseEnvInt((char *) "SIZE");

    minTask = parseEnvInt((char *) "MINIMUM_TASK");
    maxTask = parseEnvInt((char *) "MAXIMUM_TASK");

    minimumCountTasks = parseEnvInt((char *) "DELTA");

    debug = parseEnvBool((char *) "DEBUG");
}

int Config::parseEnvInt(char *variable) {
    auto value = getenv(variable);
    return std::atoi(value);
}

bool Config::parseEnvBool(char *variable) {
    return parseEnvInt(variable) != 0;
}
