#include <iostream>
#include "Conf/ConfReader.h"
#include "Algo/Algo.h"
#include "Util/util.h"

#include <mpi.h>

//#define PROFILE

#ifdef PROFILE
#include "mpe.h"
#endif

double runCalculation(int rank, int size, double (*f)(double, double, double, double)) {
#ifdef PROFILE
    auto config = ConfReader(false);
#else
    auto config = ConfReader(true);
#endif
    int countZ = countOfLines(config.Nz, rank, size);
    int firstZ = firstLine(config.Nz, rank, size);
    // +2 чтобы крайнии значения попадали
    int countElements = config.Nx * config.Ny * (countZ + 2);
    int onePanSize = config.Nx * config.Ny;
    Algo algo = Algo(config, f, rank, size, countZ, firstZ, countElements);

#ifdef PROFILE
    int calculationBegin = MPE_Log_get_event_number();
    int calculationEnd = MPE_Log_get_event_number();
    MPE_Describe_state(calculationBegin, calculationEnd, "calculation", "green");
#endif

    MPI_Request req[4];
    double maximumEpsilon, localEpsilon;
    do {

#ifdef PROFILE
        MPE_Log_event(calculationBegin, 0, NULL);
#endif
        algo.calculate(1, 2);
#ifdef PROFILE
        MPE_Log_event(calculationEnd, 0, NULL);
#endif
        if (rank != 0) {
            // Отправляем свое значение с верхнего краю
            MPI_Isend(
                    algo.getDataPointer(1), onePanSize, MPI_DOUBLE,
                    rank - 1, 0, MPI_COMM_WORLD, &req[0]
            );
            // Получаем чужое значение с верхнего краю, записываем его в дополнительный "поганый" блинчик
            MPI_Irecv(
                    algo.getDataPointer(0), onePanSize, MPI_DOUBLE,
                    rank - 1, 0, MPI_COMM_WORLD, &req[1]
            );
        }

#ifdef PROFILE
        MPE_Log_event(calculationBegin, 0, NULL);
#endif
        algo.calculate(countZ, countZ + 1);
#ifdef PROFILE
        MPE_Log_event(calculationEnd, 0, NULL);
#endif
        if (rank != size - 1) {
            // Отправляем свое значение с нижнего краю
            MPI_Isend(algo.getDataPointer(countZ), onePanSize, MPI_DOUBLE,
                      rank + 1, 0, MPI_COMM_WORLD, &req[2]
            );
            // Получаем чужое значение с нижниего краю, записываем его в "поганый" блинчик
            MPI_Irecv(
                    algo.getDataPointer(countZ + 1), onePanSize, MPI_DOUBLE,
                    rank + 1, 0, MPI_COMM_WORLD, &req[3]
            );
        }
#ifdef PROFILE
        MPE_Log_event(calculationEnd, 0, NULL);
#endif
        algo.calculate(2, countZ);
#ifdef PROFILE
        MPE_Log_event(calculationEnd, 0, NULL);
#endif
        if (rank != 0) {
            MPI_Wait(&req[0], MPI_STATUS_IGNORE);
            MPI_Wait(&req[1], MPI_STATUS_IGNORE);
        }

        if (rank != size - 1) {
            MPI_Wait(&req[2], MPI_STATUS_IGNORE);
            MPI_Wait(&req[3], MPI_STATUS_IGNORE);
        }

        algo.swapArrays();

        localEpsilon = algo.getEpslion(countZ);
        MPI_Allreduce(&localEpsilon, &maximumEpsilon, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    } while (maximumEpsilon >= config.epsilon);


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
    }
    delete[] res;
    delete[] counts;
    delete[] displs;
    return maxDelta;
}

int main() {
    // funciton to edit
    auto srcFunction = [](double x, double y, double z, double a) {
        return 6 - a * vectorSize(x, y, z);
    };

    int rank, size;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double beginTime = MPI_Wtime();
    double result = runCalculation(rank, size, srcFunction);
    double endTime = MPI_Wtime();
    double resultTime = endTime - beginTime;
    double maxTime;

    MPI_Reduce(
            &resultTime, &maxTime,
            1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD
    );
    if (rank == 0) {
        std::cout << maxTime << std::endl;
        std::cout << "delta = " << result << std::endl;
    }

    MPI_Finalize();

    return 0;
}