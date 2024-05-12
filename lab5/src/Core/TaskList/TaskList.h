#ifndef LAB5_TASKLIST_H
#define LAB5_TASKLIST_H


#include <queue>

class TaskList {
public:
    TaskList();

    /*
     * Returns count tasks in queue
     */
    int countTasks() const;

    /*
     * returns first task in queue
     */
    int getFirstTask();

    /*
     * return last task in queue
     */
    int getLastTask();

    /*
     * add a task to the end of the queue
     */
    void addTask(int task);

    /*
     * return true, if task queue is empty.
     */
    bool isEmpty() const;

    /*
     * dumpTasks(int count, int *dest)
     * get <count> tasks from end of queue and write down in <destination>
     */
    void dumpTasks(int count, int *destination);

    /*
     * loadTasks(int count, int *dest)
     *  write down <count> tasks from <source> to end of queue
     */
    void loadTasks(int count, int *source);

    /*
     * print all queue in stdin
     */
    void print();

private:
    std::deque<int> tasks{};
};


#endif
