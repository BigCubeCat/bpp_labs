#include "Core.h"
#include "Storage.h"

void Core::calculate() {
    if (taskList.isEmpty()) {
        return;
    }
    auto task = taskList.getFirstTask();
    if (taskList.isEmpty()) {
        imBusy = false;
    }
    storage.addValue(std::to_string(calcPi(task)));
}


Core::Core(int rank, int countTasks, int minimumTask, int maximumTask) : rank(rank) {
    storage = Storage(countTasks, false);
    taskList = TaskList();
    taskList.generateRandomList(rank, countTasks, minimumTask, maximumTask);
}

bool Core::isBusy() {
    return imBusy;
}

std::string Core::toString() const {
    return "rank " + std::to_string(rank) + "\n" + storage.toString();
}

