#include "Storage.h"

Storage::Storage(int size) : maxSize(size) {
}

void Storage::addValue(const std::string &key, const std::string &value) {
    queue.push(key);
    map[key] = value;

    if (queue.size() > maxSize) {
        removeOldest();
    }
}

void Storage::removeOldest() {
    while (queue.size() > maxSize) {
        auto k = queue.front();
        queue.pop();
        map.erase(k);
    }
}

std::string Storage::getValue(const std::string &key) {
    if (map.find(key) != map.end()) {
        return map[key];
    }
    return "";
}

std::string Storage::toString() {
    return queue.front() + " " + getValue(queue.front());
}
