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

void TaskList::generateCurrentTask(int rank, int size, int minimum, int maximum) {
    for (int i = 0; i < size; ++i) {
        addTask(2 * (rank + 1) + i);
    }
    std:: cout << rank << tasks.front() << std::endl;
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
