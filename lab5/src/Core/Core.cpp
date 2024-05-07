#include <iostream>
#include "Core.h"

void Core::calculate() {
    if (taskList.isEmpty()) {
        return;
    }
    auto task = taskList.getFirstTask();
    std::cout << "COunt tasks at " << rank << " " << taskList.countTasks() << std::endl;
    if (taskList.isEmpty()) {
        imBusy = false;
    }
    storage.addValue(std::to_string(calcPi(task)));
}

Core::Core(int rank, int countTasks, int minimumTask, int maximumTask) :
        rank(rank),
        storage(Storage(countTasks, false)) {
    storage = Storage(countTasks, false);
    taskList.generateRandomList(rank, countTasks, minimumTask, maximumTask);
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

int Core::countTaskToDelegate() {
    return taskList.countTasks() / 2;
}
