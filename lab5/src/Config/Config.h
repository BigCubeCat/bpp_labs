#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


class Config {
public:
    Config(int argc, char *argv[]);
    int defaultCountTasks = 10;
    bool useBalance = false;
    int syncDelay = 10;
    int storeSize = 100;

    int minTask = 0;
    int maxTask = 2;

    int minimumCountTasks = 5;

    bool debug = false;

    bool usingEnv;
private:
    void parseEnv();
    static int parseEnvInt(char *variable);
    static bool parseEnvBool(char *variable);
};

#endif
