#include <iostream>
#include "runner/runner.h"


int main(int argc, char **argv) {

    bool debug = false;

    int dims[2] = {0, 0};

    if (argc >= 4) {
        debug = std::string(argv[3]) == "debug";
       if (argc == 6) {
           dims[0] = std::atoi(argv[4]);
           dims[1] = std::atoi(argv[5]);
       }
    } else if (argc < 3) {
        std::cout << "invalid parameters;\nUsage: mpirun -np <n> ./matmult <input file> <output file>\n";
        return 1;
    }
    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];

    double resultTime = RunMultiplication(inputFileName, outputFileName, dims, debug);
    if (resultTime > 0) {
        std::cout << resultTime << std::endl;
    }

    return 0;
}
