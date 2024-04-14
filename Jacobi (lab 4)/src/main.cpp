#include <iostream>
#include "Conf/ConfReader.h"
#include "Algo/Algo.h"


int main() {
    ConfReader config = ConfReader();

    auto f = [](double x, double y, double z, double a) {
        return 6 - a * Vector3(x, y, z).size();
    };

    Algo algo = Algo(config, f);

    std:: cout << f(1, 1, 1, 6) << std::endl;

    return 0;
}