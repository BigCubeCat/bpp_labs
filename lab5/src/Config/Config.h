#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


class Config {
public:
    Config(int argc, char *argv[]);

    int defaultCountTasks = 20;
    bool useBalance = true;
    int syncDelay = 60;
    int storeSize = 100;

    int minTask = 1;
    int maxTask = 200000000;

    int minimumCountTasks = 1;

    bool debug = true;

    bool usingEnv = false;
private:
    void parseEnv();

    static int parseEnvInt(char *variable);

    static bool parseEnvBool(char *variable);
};

#endif
