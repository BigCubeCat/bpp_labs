#include "Task.h"

#include <csignal>

int doTask(int count) {
    sleep(count);
    return count;
}
