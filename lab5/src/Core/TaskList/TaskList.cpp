#include <iostream>

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

bool TaskList::isEmpty() const {
    return (countTasks() == 0);
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
    for (int task : tasks) {
        std::cout << task << " ";
    }
    std::cout << std::endl;
}
