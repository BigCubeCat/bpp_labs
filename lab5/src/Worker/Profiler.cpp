#include <string>
#include "Profiler.h"

void Profiler::calcDisbalance() {
    disbalance = (maxTime - minTime) / maxTime;
}

std::string Profiler::toString(int defaultCountTasks, bool useBalance) const {
    auto dis = static_cast<int>(disbalance * 10000);
    auto value = std::to_string(dis / 100) + "." + std::to_string(dis % 100);
    std::string result;
    result += "count tasks        " + std::to_string(defaultCountTasks) + "\n";
    result += "maximum time       " + std::to_string(maxTime) + "\n";
    result += "minimum time       " + std::to_string(minTime) + "\n";
    result += "disbalance         " + value + "%\n";
    result += "balanced           ";
    result += (useBalance) ? " true" : "false";
    result += "\n";
    return result;
}
