#include <float.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_ITERATIONS = 10000;
const double EPSILON = 0.000001;
const double TAU = 0.00001;
const int N = 700;


int countOfLines(int n, int rank, int size) {
    return n / size + (rank < n % size);
}

void init(double *matrix, double *bVector, double *xVector, int n) {
    for (int i = 0; i < n; ++i) {
        xVector[i] = 0;
        bVector[i] = n + 1;
        for (int j = 0; j < n; ++j) {
            matrix[i * n + j] = 1 + (i == j);
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *linesCount = (int *) malloc(size * sizeof(int));
    int *firstLines = (int *) malloc(size * sizeof(int));
    firstLines[0] = 0;

    for (int i = 0; i < size; ++i) {
        linesCount[i] = countOfLines(N, i, size);
        if (i > 0) firstLines[i] = firstLines[i - 1] + linesCount[i - 1];
    }

    double *matrix = (double *) malloc(sizeof(double) * N *
                                       (rank == 0 ? N : linesCount[rank]));
    double *xVector = (double *) malloc(sizeof(double) * N);
    double *bVector =
            (double *) malloc(sizeof(double) * (rank == 0 ? N : linesCount[rank]));

    int buffSize = sizeof(double) * (linesCount[0] * N + N + linesCount[0]);
    void *buff = (void *) malloc(buffSize);

    if (rank == 0) {
        init(matrix, bVector, xVector, N);
        for (int i = 1; i < size; ++i) {
            int pos = 0;
            MPI_Pack(matrix + N * firstLines[i], N * linesCount[i], MPI_DOUBLE,
                     buff, buffSize, &pos, MPI_COMM_WORLD);
            MPI_Pack(xVector, N, MPI_DOUBLE, buff, buffSize, &pos,
                     MPI_COMM_WORLD);
            MPI_Pack(bVector + firstLines[i], linesCount[i], MPI_DOUBLE, buff,
                     buffSize, &pos, MPI_COMM_WORLD);
            MPI_Send(buff, buffSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int pos = 0;
        MPI_Recv(buff, buffSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Unpack(buff, buffSize, &pos, matrix, N * linesCount[rank],
                   MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(buff, buffSize, &pos, xVector, N, MPI_DOUBLE,
                   MPI_COMM_WORLD);
        MPI_Unpack(buff, buffSize, &pos, bVector, linesCount[rank], MPI_DOUBLE,
                   MPI_COMM_WORLD);
    }
    free(buff);

    double bLen = 0;
    if (rank == 0) {
        for (int i = 0; i < N; ++i) {
            bLen += bVector[i] * bVector[i];
        }
    }
    bLen *= EPSILON * EPSILON;

    double *xVectorNew = (double *) calloc(linesCount[rank], sizeof(double));
    double *d;
    if (rank == 0) {
        d = (double *) malloc(sizeof(double) * size);
    }

    int flag = 1;
    int useTau = 0;
    double tau = TAU;
    double prevParam = DBL_MAX;
    int countIter = 0;

    double *sVector = malloc(linesCount[0] * sizeof(double));
    double *xVectorPart = (double *) malloc(linesCount[0] * sizeof(double));
    int right_rank = (rank + 1) % size;
    int left_rank = (rank - 1 + size) % size;

    for (; flag && (countIter < MAX_ITERATIONS); ++countIter) {
        double dd = 0;
        for (int i = 0; i < linesCount[rank]; ++i) {
            sVector[i] = -bVector[i];
            xVectorPart[i] = xVectorNew[i];
        }
        for (int partId = 0; partId < size; ++partId) {
            MPI_Status st;
            // copy answer vector to copys;
            MPI_Sendrecv_replace(xVectorPart, linesCount[0], MPI_DOUBLE,
                                 left_rank, 0, right_rank, 0, MPI_COMM_WORLD,
                                 &st);
            for (int i = 0; i < linesCount[rank]; ++i) {
                for (int j = 0; j < linesCount[partId]; ++j) {
                    sVector[i] += matrix[i * N + j + firstLines[partId]] *
                                  xVectorPart[j]; // Вектор xVector должен иметь другйо индексc
                }
            }
        }
        for (int i = 0; i < linesCount[rank]; ++i) {
            dd += sVector[i] * sVector[i];
            xVectorNew[i] = xVectorPart[i] - tau * sVector[i];
        }

        printf("rank = %d; dd = %f\n", rank, dd);
        // Собираем вектор по кусочкам
        MPI_Allgatherv(xVectorNew, linesCount[rank], MPI_DOUBLE, xVector,
                       linesCount, firstLines, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Gather(&dd, 1, MPI_DOUBLE, d, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            double endParam = 0;
            for (int i = 0; i < size; ++i) {
                endParam += d[i];
            }
            if (prevParam <= endParam) {    // условие смены знака скаляра.
                if (useTau) {
                    flag = 0;    // Очевидно, что на прямой к числу можно
                } else {
                    tau *= -1;
                    useTau = 1;
                    MPI_Bcast(&tau, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast(&useTau, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                }
            }
            prevParam = endParam;
            flag = flag && endParam > bLen;
        }

        // выходим все
        MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("\n%f\n", xVector[0]);
        printf("count iterations = %d\n", countIter);
        printf("\n--------------\n");
        printf("Count of MPI process: %d\n", size);
    }

    free(matrix);
    free(xVector);
    free(xVectorNew);
    free(bVector);
    if (rank == 0) {
        free(d);
    }
    free(linesCount);
    free(firstLines);

    MPI_Finalize();
    return 0;
}
