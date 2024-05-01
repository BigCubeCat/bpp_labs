#include "Config.h"

#include <iostream>

Config::Config(int argc, char **argv) {
    if (argc == 2) {
        if (argv[1] == "env") {
            std::cout << "Using .env\n";
            parseEnv();
            return;
        }
    }
    std::cout << "Using default config\n";
}

void Config::parseEnv() {
    defaultCountTasks = parseEnvInt("COUNT_TASKS");
    useBalance = parseEnvBool("BALANCE");
    calcDisbalance = parseEnvBool("PROFILE");
    syncDelay = parseEnvInt("SYNC");
    storeSize = parseEnvInt("SIZE");

    minTask = parseEnvInt("MINIMUM_TASK");
    maxTask = parseEnvInt("MAXIMUM_TASK");
}

int Config::parseEnvInt(char *variable) {
    return std::atoi(getenv(variable));
}

bool Config::parseEnvBool(char *variable) {
    std::string value  = getenv(variable);
    return (value == "true" || value == "True" || value == "1");
}
