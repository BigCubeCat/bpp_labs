#include "Config.h"

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
    defaultCountTasks = parseEnvInt("COUNT_TASKS");
    useBalance = parseEnvBool("BALANCE");
    timeout = parseEnvInt("TIMEOUT");
    swapSize = parseEnvInt("SWAP_SIZE");
    critical = parseEnvInt("CRIT");
    debug = parseEnvBool("DEBUG");
}

int Config::parseEnvInt(const std::string &variable) {
    return std::atoi(parseEnvVar(variable).c_str());
}

bool Config::parseEnvBool(const std::string &variable) {
    return parseEnvVar(variable) == "true";
}

std::string Config::parseEnvVar(const std::string &variable) {
    char *val = getenv(variable.c_str());
    return (val == nullptr) ? std::string("") : std::string(val);
}
