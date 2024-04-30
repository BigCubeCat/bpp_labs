#ifndef LAB5_STORAGE_H
#define LAB5_STORAGE_H


#include <map>
#include <queue>
#include <string>

class Storage {
public:
    Storage(int size);

    void addValue(const std::string &key, const std::string &value);

    std::string getValue(const std::string &key);

    std::string toString();

private:
    int maxSize;
    int size;
    std::map<std::string, std::string> map{};
    std::queue<std::string> queue{};

    void removeOldest();
};


#endif //LAB5_STORAGE_H
