#ifndef LAB5_CORE_H
#define LAB5_CORE_H

#include <string>
#include <vector>

#include "Task/Task.h"
#include "TaskList/TaskList.h"
#include "Storage/Storage.h"

class Core {
private:
    int rank, startCount, cheapTaskCount;
    TaskList taskList{};
    Storage storage;

    bool imBusy = true; // есть задачи кроме текущей
public:
    Core(int rank, int *input, int countTasks);

    std::string toString();

    void calculate();

    bool isBusy() const;

    int countTasks();

    void print();

    // изменеие списка задач
    void dumpTasks(int count, int *dest);

    void loadTasks(int count, int *source);

};


#endif
