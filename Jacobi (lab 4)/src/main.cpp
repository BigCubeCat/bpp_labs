#include <iostream>
#include "Conf/ConfReader.h"
#include "Algo/Algo.h"
#include "Util/util.h"

#include <mpi.h>

double runCalculation(int rank, int countLayers) {
    MPI_Request req[4];
    double delta = 0;
    ConfReader config = ConfReader();
    Algo algo = Algo(
            config,
            [](double x, double y, double z, double a) {
                return 6 - a * Vector3(x, y, z).size();
            },
            rank
    );

    int size, extendSize;
    int commonLayerSize = config.Nz / countLayers;
    size = rank == countLayers - 1 ? (config.Nz - commonLayerSize * rank) : commonLayerSize;
    extendSize = size + 2;

    int z = rank * size - 1;

    double *omega;

    int leftNeighbor = rank - 1;
    int rightNeighbor = rank + 1;

    double curDelta;
    while (!algo.isStopped()) {
        curDelta = 0;

        if (rank != 0) {
            MPI_Isend(algo.tempData + algo.layerSize, algo.layerSize, MPI_DOUBLE, leftNeighbor, 0, MPI_COMM_WORLD,
                      &req[0]);
            MPI_Irecv(algo.tempData, algo.layerSize, MPI_DOUBLE, leftNeighbor, 0, MPI_COMM_WORLD, &req[1]);
        }

        if (rank != countLayers - 1) {
            MPI_Isend(algo.tempData + algo.layerSize * size, algo.layerSize, MPI_DOUBLE, rightNeighbor, 0,
                      MPI_COMM_WORLD,
                      &req[2]);
            MPI_Irecv(algo.tempData + algo.layerSize * (size + 1), algo.layerSize, MPI_DOUBLE, rightNeighbor, 0,
                      MPI_COMM_WORLD, &req[3]);
        }

        for (int i = 2; i < size; i++) {
            algo.calcNextPhi(z, i);
            curDelta = algo.maximumDifference;
        }

        if (rank != countLayers - 1) {
            MPI_Wait(&req[3], MPI_STATUS_IGNORE);
            MPI_Wait(&req[2], MPI_STATUS_IGNORE);
        }
        if (rank != 0) {
            MPI_Wait(&req[1], MPI_STATUS_IGNORE);
            MPI_Wait(&req[0], MPI_STATUS_IGNORE);
        }

        algo.calcNextPhi(z, 1);
        curDelta = maximumDouble(algo.maximumDifference, curDelta);
        algo.calcNextPhi(z, size);
        curDelta = maximumDouble(algo.maximumDifference, curDelta);
        algo.swapArrays();

        // TODO: IAllreduce
        MPI_Allreduce(&curDelta, &delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        algo.checkStopped();
    }


    return algo.getMaxDelta();
}

int main() {
    int rank, countLayers;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &countLayers);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double startTime = MPI_Wtime();
    double maximumDelta = runCalculation(rank, countLayers);
    double finishTime = MPI_Wtime();

    std::cout << "rank = " << rank << " time = " << (finishTime - startTime) << " result = " << maximumDelta << "\n";

    MPI_Finalize();
    return 0;
}