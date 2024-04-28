#include "Config.h"

#include <iostream>

Config::Config(int argc, char **argv) {
    if (argc == 2) {
        if (argv[1] == "env") {
            std::cout << "Loading from env\n";
            parseEnv();
        }
    }
}

void Config::parseEnv() {
    store = parseEnvInt("STORE");
    server = parseEnvInt("SERVER");
    storeSize = parseEnvInt("SIZE");
}

int Config::parseEnvInt(char *variable) {
    return std::atoi(getenv(variable));
}
