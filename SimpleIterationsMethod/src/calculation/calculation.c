#include <malloc.h>
#include <mpi.h>
#include "./calculation.h"

int countOfLines(int n, int rank, int size) {
    return n / size + (rank < n % size);
}

void initLinesSettings(int *linesCount, int *firstLines, int size, int n) {
    firstLines[0] = 0;
    linesCount[0] = countOfLines(n, 0, size);

    for (int i = 1; i < size; ++i) {
        linesCount[i] = countOfLines(n, i, size);
        firstLines[i] = firstLines[i - 1] + linesCount[i - 1];
    }
}

void prepare(CtxData *data, int size) {
    data->linesCount = (int *) malloc(size * sizeof(int));
    data->firstLines = (int *) malloc(size * sizeof(int));
}

void syncStartData(CtxData *data, int rank, int size) {
    int buffSize = sizeof(double) * (data->linesCount[0] * data->n + data->n + data->linesCount[0]);
    void *buff = (void *) malloc(buffSize);

    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            int pos = 0;
            MPI_Pack(data->matrix + data->n * data->firstLines[i], (int) data->n * data->linesCount[i], MPI_DOUBLE,
                     buff, buffSize, &pos, MPI_COMM_WORLD);
            MPI_Pack(data->x_vector, (int) data->n, MPI_DOUBLE, buff, buffSize, &pos,
                     MPI_COMM_WORLD);
            MPI_Pack(data->b_vector + data->firstLines[i], data->linesCount[i], MPI_DOUBLE, buff,
                     buffSize, &pos, MPI_COMM_WORLD);
            MPI_Send(buff, buffSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int pos = 0;
        MPI_Recv(buff, buffSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Unpack(buff, buffSize, &pos, data->matrix, (int) data->n * data->linesCount[rank],
                   MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(buff, buffSize, &pos, data->x_vector, (int) data->n, MPI_DOUBLE,
                   MPI_COMM_WORLD);
        MPI_Unpack(buff, buffSize, &pos, data->b_vector, data->linesCount[rank], MPI_DOUBLE,
                   MPI_COMM_WORLD);
    }
    free(buff);
}