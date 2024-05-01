#include "Config.h"

#include <iostream>

Config::Config(int argc, char **argv) {
    std::cout << argc << " " << argv << std::endl;
    if (argc == 2) {
        std::cout << "Using .env\n";
        parseEnv();
        return;
    }
    std::cout << "Using default config\n";
}

void Config::parseEnv() {
    defaultCountTasks = parseEnvInt((char *) "COUNT_TASKS");
    useBalance = parseEnvBool((char *) "BALANCE");
    calcDisbalance = parseEnvBool((char *) "PROFILE");
    syncDelay = parseEnvInt((char *) "SYNC");
    storeSize = parseEnvInt((char *) "SIZE");

    minTask = parseEnvInt((char *) "MINIMUM_TASK");
    maxTask = parseEnvInt((char *) "MAXIMUM_TASK");

    minimumCountTasks = parseEnvInt((char *) "JOB_UNIT");

    debug = parseEnvBool((char *) "DEBUG");
}

int Config::parseEnvInt(char *variable) {
    auto value = getenv(variable);
    return std::atoi(value);
}

bool Config::parseEnvBool(char *variable) {
    return parseEnvInt(variable) != 0;
}
