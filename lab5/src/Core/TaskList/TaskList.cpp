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

void TaskList::generateCurrentTask(int rank, int size) {
    int m = (rank % 2) * rank;
    for (int i = 0; i < size; ++i) {
        addTask(m * (rank + 1) + 2 * i);
    }
    std::cout << rank;
    print();
}

void TaskList::dumpTasks(int count, int *destination) {
    for (int i = 0; i < count; ++i) {
        destination[i] = getLastTask();
    }
}

void TaskList::loadTasks(int count, int *source) {
    for (int i = 0; i < count; ++i) {
        addTask(source[i]);
    }
}

void TaskList::print() {
    for (int i = 0; i < tasks.size(); ++i) {
        std::cout << tasks[i] << " ";
    }
    std::cout << std::endl;
}
