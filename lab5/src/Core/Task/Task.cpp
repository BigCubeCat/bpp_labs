#include "Task.h"

#include <unistd.h>

int doTask(int count) {
    sleep(count);
    return count;
}
