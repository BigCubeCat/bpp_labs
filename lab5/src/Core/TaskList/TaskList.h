#ifndef LAB5_TASKLIST_H
#define LAB5_TASKLIST_H


#include <queue>

class TaskList {
public:
    TaskList();

    int countTasks() const;

    int getFirstTask();

    int getLastTask();

    void addTask(int task);

    bool isEmpty();

    void generateCurrentTask(int rank, int size);

    void dumpTasks(int count, int *destination);

    void loadTasks(int count, int *source);

    void print();

private:
    std::deque<int> tasks{};
};


#endif
