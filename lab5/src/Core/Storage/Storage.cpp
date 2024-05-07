#include <iostream>
#include "Storage.h"

Storage::Storage(int size, bool limit) : maxSize(size), limitedSize(limit) {
}

void Storage::addValue(const std::string &value) {
    queue.push(value);

    if (limitedSize && queue.size() > maxSize) {
        removeOldest();
    }
}

void Storage::removeOldest() {
    while (queue.size() > maxSize) {
        auto k = queue.front();
        queue.pop();
        std::cout << "remove " << k << std::endl;
    }
}

std::string Storage::toString() {
    std::string result = "storage (" + std::to_string(queue.size()) + "): ";
    while (!queue.empty()) {
        result += queue.front() + " ";
        queue.pop();
    }
    result += "\n";
    return result;
}
