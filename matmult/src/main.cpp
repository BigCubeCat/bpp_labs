#include <iostream>
#include "runner/runner.h"


int main(int argc, char **argv) {
    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];
    bool debug = false;
    if (argc == 4) {
        debug = std::string(argv[3]) == "debug";
    } else if (argc < 3) {
        std::cout << "invalid parameters;\nUsage: mpirun -np <n> ./matmult <input file> <output file>\n";
    }

    double resultTime = RunMultiplication(inputFileName, outputFileName, debug);
    if (resultTime > 0) {
        std::cout << resultTime << std::endl;
    }

    return 0;
}
