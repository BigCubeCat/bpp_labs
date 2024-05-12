#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


#include <string>

class Config {
public:
    Config(int argc, char *argv[]);

    // .env variables
    int defaultCountTasks = 20;
    bool useBalance = true;
    int timeout = 500;
    int swapSize = 1;
    int critical = 3;
    bool debug = true;

    bool usingEnv = false;
private:
    void parseEnv();

    static std::string parseEnvVar(const std::string &variable);

    static int parseEnvInt(const std::string &variable);

    static bool parseEnvBool(const std::string &variable);
};

#endif
