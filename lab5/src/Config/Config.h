#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


class Config {
public:
    Config(int argc, char *argv[]);

    int defaultCountTasks = 19;
    bool useBalance = false;
    int syncDelay = 60;
    int storeSize = 100;

    int minTask = 100;
    int maxTask = 100000000;

    int minimumCountTasks = 1;

    bool debug = false;

    bool usingEnv = false;
private:
    void parseEnv();

    static int parseEnvInt(char *variable);

    static bool parseEnvBool(char *variable);
};

#endif
