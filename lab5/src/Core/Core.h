#ifndef LAB5_CORE_H
#define LAB5_CORE_H

#include <string>
#include <vector>

#include "Task/Task.h"
#include "TaskList/TaskList.h"
#include "Storage/Storage.h"

class Core {
private:
    int rank;
    TaskList taskList{};
    Storage storage;
public:
    Core(int rank, int *input, int countTasks);

    std::string toString();

    /*
     * do Task.h
     */
    void calculate();

    /*
     * returns count tasks in queue
     */
    int countTasks();

    void print();

    /*
     * see TaskList.h
     */
    void dumpTasks(int count, int *dest);

    /*
     * see TaskList.h
     */
    void loadTasks(int count, int *source);

};


#endif
