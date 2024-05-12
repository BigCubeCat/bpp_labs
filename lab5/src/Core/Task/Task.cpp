#include "Task.h"

#include <unistd.h>

int doTask(int count) {
    usleep(count * 1000);
    return count;
}
