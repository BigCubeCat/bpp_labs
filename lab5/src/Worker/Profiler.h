#ifndef LAB5_PROFILER_H
#define LAB5_PROFILER_H


struct Profiler {
    double timeSpent = 0;
    double disbalance = -1;
    double maxTime = -1;
    double minTime = -1;

    void calcDisbalance();

    std::string toString(int defaultCountTasks, bool useBalance) const;
};


#endif
