#ifndef LAB5_PARSER_H
#define LAB5_PARSER_H


class Config {
public:
    Config(int argc, char *argv[]);
    int store = 1;
    int server = 0;
    int storeSize = 100;
private:
    void parseEnv();
    static int parseEnvInt(char *variable);
};

#endif
