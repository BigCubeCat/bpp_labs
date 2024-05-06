#include "Config.h"

#include <iostream>

Config::Config(int argc, char **argv) {
    if (argc == 2) {
        usingEnv = true;
        parseEnv();
        return;
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
