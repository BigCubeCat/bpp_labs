#include <iostream>
#include "Core.h"

void Core::calculate() {
    if (taskList.isEmpty()) {
        return;
    }
    doTask(taskList.getFirstTask());
}

Core::Core(int rank, int *input, int countTasks) :
        rank(rank),
        storage(Storage(countTasks, false)) {
    storage = Storage(countTasks, false);
    loadTasks(countTasks, input);
}

std::string Core::toString() {
    return "rank " + std::to_string(rank) + "\n" + storage.toString();
}

void Core::dumpTasks(int count, int *dest) {
    taskList.dumpTasks(count, dest);
}

void Core::loadTasks(int count, int *source) {
    taskList.loadTasks(count, source);
}

int Core::countTasks() {
    return taskList.countTasks();
}

void Core::print() {
    taskList.print();
}
