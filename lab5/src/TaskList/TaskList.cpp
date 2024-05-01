#include <cstdlib>
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

void TaskList::generateRandomList(int size, int minimum, int maximum) {
    int rangeSize = maximum - minimum;
    srand(0);
    auto randrange = [&rangeSize, &minimum]() {
        return minimum + static_cast<int>(rand() * rangeSize);
    };
    for (int i = 0; i < size; ++i) {
        addTask(randrange());
    }
}
