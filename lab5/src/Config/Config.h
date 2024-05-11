#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


#include <string>

class Config {
public:
    Config(int argc, char *argv[]);

    // .env variables
    int defaultCountTasks = 30;
    bool useBalance = true;
    int timeout = 100;
    int swapSize = 2;
    int critical = 2;
    bool debug = true;

    bool usingEnv = false;
private:
    void parseEnv();

    static int parseEnvInt(const std::string &variable);

    static bool parseEnvBool(const std::string &variable);
};

#endif
