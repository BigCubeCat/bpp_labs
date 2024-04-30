#include <iostream>
#include "Conf/ConfReader.h"
#include "Algo/Algo.h"
#include "Util/util.h"

#include <mpi.h>


double runCalculation(int rank, int size, double (*f)(double, double, double, double), const ConfReader &config) {
    double beginTime = MPI_Wtime();
    int countZ = countOfLines(config.Nz, rank, size);
    int firstZ = firstLine(config.Nz, rank, size);
    // +2 чтобы крайнии значения попадали
    int countElements = config.Nx * config.Ny * (countZ + 2);
    int onePanSize = config.Nx * config.Ny;
    Algo algo = Algo(config, f, countZ, firstZ, countElements);

    MPI_Request req[4];
    double maximumEpsilon, localEpsilon;
    int index;
    do {
        index = 0;
        algo.calculate(1, 2);
        if (rank != 0) {
            // Отправляем свое значение с верхнего краю
            MPI_Isend(
                    algo.getDataPointer(1), onePanSize, MPI_DOUBLE,
                    rank - 1, 0, MPI_COMM_WORLD, &req[index++]
            );
            // Получаем чужое значение с верхнего краю, записываем его в дополнительный "поганый" блинчик
            MPI_Irecv(
                    algo.getDataPointer(0), onePanSize, MPI_DOUBLE,
                    rank - 1, 0, MPI_COMM_WORLD, &req[index++]
            );
        }

        algo.calculate(countZ, countZ + 1);
        if (rank != size - 1) {
            // Отправляем свое значение с нижнего краю
            MPI_Isend(algo.getDataPointer(countZ), onePanSize, MPI_DOUBLE,
                      rank + 1, 0, MPI_COMM_WORLD, &req[index++]
            );
            // Получаем чужое значение с нижниего краю, записываем его в "поганый" блинчик
            MPI_Irecv(
                    algo.getDataPointer(countZ + 1), onePanSize, MPI_DOUBLE,
                    rank + 1, 0, MPI_COMM_WORLD, &req[index++]
            );
        }

        algo.calculate(2, countZ);

        MPI_Waitall(index, req, MPI_STATUS_IGNORE);

        algo.swapArrays();

        localEpsilon = algo.getEpslion(countZ);
        MPI_Allreduce(&localEpsilon, &maximumEpsilon, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    } while (maximumEpsilon >= config.epsilon);
    MPI_Barrier(MPI_COMM_WORLD);
    double endTime = MPI_Wtime();

    int *counts = new int[size];
    int *displs = new int[size];

    for (int i = 0; i < size; ++i) {
        counts[i] = countOfLines(config.Nz, i, size) * onePanSize;
        displs[i] = firstLine(config.Nz, i, size) * onePanSize;
    }

    int resultSize = (rank == 0) ? (onePanSize * config.Nz) : 0;
    auto *res = new double[resultSize];

    MPI_Gatherv(
            algo.getDataPointer(1), counts[rank], MPI_DOUBLE, res,
            counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD
    );

    double maxDelta = 0;
    if (rank == 0) {
        maxDelta = calculateDelta(config, res);
        std::cout << "maxDelta = " << maxDelta << std::endl;
    }
    delete[] res;
    delete[] counts;
    delete[] displs;
    return endTime - beginTime;
}

int main(int argc, char **argv) {
    // funciton to edit
    auto srcFunction = [](double x, double y, double z, double a) {
        return 6 - a * vectorSize(x, y, z);
    };

    int rank, size;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto config = ConfReader((argc == 2));
    double result = runCalculation(rank, size, srcFunction, config);
    if (rank == 0) {
        std::cout << "time = " << result << std::endl;
    }

    MPI_Finalize();

    return 0;
}