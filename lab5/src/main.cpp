#include <iostream>
#include "Config.h"
#include "Worker/Worker.h"

int main(int argc, char **argv) {
    auto conf = Config(argc, argv);
    auto worker = Worker(conf);
    worker.Run();
    std::cout << worker.getResult();
    return 0;
}
