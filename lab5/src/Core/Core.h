#ifndef LAB5_CORE_H
#define LAB5_CORE_H

#include <string>
#include <vector>

#include "pi/pi.h"
#include "TaskList/TaskList.h"
#include "Storage/Storage.h"

class Core {
private:
    int rank;
    TaskList taskList{};
    Storage storage;

    bool imDone = false; // все задачи решениы
    bool imBusy = true; // есть задачи кроме текущей
public:
    Core(int rank, int countTasks, int minimumTask, int maximumTask);

    std::string toString();

    void calculate();

    bool isBusy();

    bool isDone();
};


#endif
