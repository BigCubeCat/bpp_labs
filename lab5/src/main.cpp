#include <iostream>
#include "Config.h"
#include "Worker/Worker.h"

int main(int argc, char **argv) {
    Config conf = Config(argc, argv);
    auto worker = Worker(conf.storeSize);
    worker.Run();
    std::cout << "res = " << worker.getResult().toString() << std::endl;
    return 0;
}
