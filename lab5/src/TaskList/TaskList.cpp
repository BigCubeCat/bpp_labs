#include <cstdlib>
#include <iostream>
#include <random>

#include "TaskList.h"

TaskList::TaskList() {
    tasks = std::deque<int>();
}

int TaskList::countTasks() const {
    return tasks.size();
}

int TaskList::getFirstTask() {
    int result = tasks.front();
    tasks.pop_front();
    return result;
}

int TaskList::getLastTask() {
    int result = tasks.back();
    tasks.pop_back();
    return result;
}

void TaskList::addTask(int task) {
    tasks.push_back(task);
}

bool TaskList::isEmpty() {
    return (countTasks() == 0);
}

void TaskList::generateRandomList(int seed, int size, int minimum, int maximum) {
    srand(seed);
    auto randPercent = []() -> double {
        return static_cast<double>(static_cast<int>(static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * 10)) / 10;
    };
    auto randrange = [&minimum, &maximum, &randPercent]() {
        return minimum + static_cast<int>(randPercent() * (maximum - minimum));
    };
    for (int i = 0; i < size; ++i) {
        addTask(randrange());
    }
}
