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

    void generateRandomList(int seed, int size, int minimum, int maximum);

    void dumpTasks(int count, int *destination);

    void loadTasks(int count, int *source);

private:
    std::deque<int> tasks{};
};


#endif