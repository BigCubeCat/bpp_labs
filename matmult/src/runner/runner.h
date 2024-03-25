#ifndef MATMULT_RUNNER_H
#define MATMULT_RUNNER_H

#include <string>

void RunMultiplication(const std::string &filename, int mpiRank, int mpiSize, bool debug);

#endif
