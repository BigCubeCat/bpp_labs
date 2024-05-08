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

    bool needMore = false; // нужно ли больше задач
    bool imBusy = true; // есть задачи кроме текущей
public:
    Core(int rank, int countTasks, int minimumTask, int maximumTask);

    std::string toString();

    void calculate();

    bool isBusy() const;

    bool needMoreTasks() const;

    void dumpTasks(int count, int *dest);

    void loadTasks(int count, int *source);

    int countTaskToDelegate();

    int countTasks();
};


#endif
