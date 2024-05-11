#include <iostream>
#include "Core.h"

void Core::calculate() {
    if (taskList.isEmpty()) {
        return;
    }
    auto task = taskList.getFirstTask();
    imBusy = taskList.countTasks() > 0;
    storage.addValue(std::to_string(doTask(task)));
}

Core::Core(int rank, int *input, int countTasks) :
        rank(rank), startCount(countTasks),
        cheapTaskCount(countTasks - countTasks / 3),
        storage(Storage(countTasks, false)) {
    storage = Storage(countTasks, false);
    loadTasks(countTasks, input);
}

bool Core::isBusy() const {
    return imBusy;
}

std::string Core::toString() {
    return "rank " + std::to_string(rank) + "\n" + storage.toString();
}

void Core::dumpTasks(int count, int *dest) {
    taskList.dumpTasks(count, dest);
    imBusy = !taskList.isEmpty();
}

void Core::loadTasks(int count, int *source) {
    taskList.loadTasks(count, source);
    imBusy = !taskList.isEmpty();
}

int Core::countTasks() {
    return taskList.countTasks();
}

void Core::print() {
    taskList.print();
}
