#include "Task.h"

#include <thread>

int doTask(int count) {
    std::this_thread::sleep_for(std::chrono::seconds(count));
    return count;
}
