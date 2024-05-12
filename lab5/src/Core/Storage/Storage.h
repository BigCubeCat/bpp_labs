#ifndef LAB5_STORAGE_H
#define LAB5_STORAGE_H


#include <queue>
#include <string>

/*
 * Storage use to save Task simulation result (useless for time::sleep)
 */
class Storage {
public:
    Storage(int size, bool limit);

    /*
     * add value in answer queue
     */
    void addValue(const std::string &value);

    std::string toString();

private:
    int maxSize;
    bool limitedSize;
    std::queue<std::string> queue{};

    void removeOldest();
};


#endif //LAB5_STORAGE_H
