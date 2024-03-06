#include <float.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_MPE
#include <mpe.h>
#endif

const int MAX_ITERATIONS = 10000;
const double EPSILON = 0.000001;
const double TAU = 0.00001;
const size_t N = 25000;

void getElapsedTime(double *xVectorNew, int startTime, int size, int rank,
                    int countIter) {
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;

    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
        printf("%f\n", maxTime);
        printf("\n--------------\n");
        printf("\n%f\n", xVectorNew[0]);
        printf("count iterations = %d\n", countIter);
        printf("Count of MPI process: %d\n", size);
        printf("\n--------------\n");
    }
}

double calc(double *matrix, double *xVector, double *bVector,
            double *xVectorNew, size_t n, double tao, size_t cnt, int first) {
    double res = 0;
    for (size_t i = 0; i < cnt; ++i) {
        double s = -bVector[i];
        for (size_t j = 0; j < n; ++j) {
            s += matrix[i * n + j] * xVector[j];
        }
        res += s * s;
        xVectorNew[i] = xVector[first + i] - tao * s;
    }
    return res;
}

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

/*
 * Конфигурация данных по процессам
 */
void initLinesSettings(int *linesCount, int *firstLines, int size, int n) {
    firstLines[0] = 0;
    linesCount[0] = countOfLines(n, 0, size);

    for (int i = 1; i < size; ++i) {
        linesCount[i] = countOfLines(n, i, size);
        firstLines[i] = firstLines[i - 1] + linesCount[i - 1];
    }
}

double calcEndValue(double *bVector, int n, double eps) {
    double res = 0;
    for (int i = 0; i < n; ++i) {
        res += bVector[i] * bVector[i];
    }
    return res * eps * eps;
}

double sumVector(double *vector, int size) {
    int result = 0;
    for (int i = 0; i < size; ++i) {
        result += vector[i];
    }
    return result;
}

int solve(double *matrix, double *xVectorPart, double *sVector, double *bVector,
          double *xVectorNew, int *linesCount, int *firstLines, int rank,
          int size, int bLen, double prevParam, double tau) {
    int countIter = 0;
    int flag = 1;
    double endParam;

    int right_rank = (rank + 1) % size;
    int left_rank = (rank - 1 + size) % size;

    for (; flag && (countIter < MAX_ITERATIONS); ++countIter) {
        double dd = 0;
        for (int i = 0; i < linesCount[rank]; ++i) {
            sVector[i] = -bVector[i];
            xVectorPart[i] = xVectorNew[i];
        }
        for (int partId = 0; partId < size; ++partId) {
            // copy answer vector to copys;
            MPI_Sendrecv_replace(xVectorPart, linesCount[0], MPI_DOUBLE,
                                 left_rank, 0, right_rank, 0, MPI_COMM_WORLD,
                                 MPI_STATUS_IGNORE);
            for (int i = 0; i < linesCount[rank]; ++i) {
                for (int j = 0; j < linesCount[partId]; ++j) {
                    sVector[i] +=
                        matrix[i * N + j + firstLines[partId]] * xVectorPart[j];
                }
            }
        }
        for (int i = 0; i < linesCount[rank]; ++i) {
            dd += sVector[i] * sVector[i];
            xVectorNew[i] = xVectorPart[i] - tau * sVector[i];
        }
        MPI_Allreduce(&dd, &endParam, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        if (prevParam <= endParam || endParam <= bLen) {
            flag = 0;
        }
        prevParam = endParam;
        flag = flag && (endParam > bLen);
    }
    return countIter;
}

int main(int argc, char **argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
#ifdef USE_MATH_LIB
    MPE_Init_log();
#endif
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double startTime = MPI_Wtime();

    /* Считаем конфигурацию себя и других */
    int *linesCount = (int *)malloc(size * sizeof(int));
    int *firstLines = (int *)malloc(size * sizeof(int));
    initLinesSettings(linesCount, firstLines, size, N);
    size_t currentSize = (rank == 0 ? N : (size_t)linesCount[rank]);
    /* выделение памяти */
    double *matrix = (double *)malloc(sizeof(double) * N * currentSize);
    double *xVector = (double *)malloc(sizeof(double) * N);
    double *bVector = (double *)malloc(sizeof(double) * currentSize);
    double *xVectorNew = (double *)calloc(linesCount[rank], sizeof(double));
    double *sVector = malloc(linesCount[0] * sizeof(double));
    double *xVectorPart = (double *)malloc(linesCount[0] * sizeof(double));

    int buffSize = sizeof(double) * (linesCount[0] * N + N + linesCount[0]);
    void *buff = (void *)malloc(buffSize);

    if (rank == 0) {
        init(matrix, bVector, xVector, N);
        for (int i = 1; i < size; ++i) {
            int pos = 0;
            MPI_Pack(matrix + N * firstLines[i], N * linesCount[i], MPI_DOUBLE,
                     buff, buffSize, &pos, MPI_COMM_WORLD);
            MPI_Pack(xVector, (int)N, MPI_DOUBLE, buff, buffSize, &pos,
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
        MPI_Unpack(buff, buffSize, &pos, xVector, (int)N, MPI_DOUBLE,
                   MPI_COMM_WORLD);
        MPI_Unpack(buff, buffSize, &pos, bVector, linesCount[rank], MPI_DOUBLE,
                   MPI_COMM_WORLD);
    }
    free(buff);

    double bLen = (rank == 0) ? calcEndValue(bVector, (int)N, EPSILON) : 0;
    double prevParam = DBL_MAX;

    int countIter =
        solve(matrix, xVectorPart, sVector, bVector, xVectorNew, linesCount,
              firstLines, rank, size, bLen, prevParam, TAU);
    countIter +=
        solve(matrix, xVectorPart, sVector, bVector, xVectorNew, linesCount,
              firstLines, rank, size, bLen, prevParam, -TAU);

    getElapsedTime(xVectorNew, startTime, size, rank, countIter);

    free(matrix);
    free(xVector);
    free(xVectorNew);
    free(bVector);
    free(linesCount);
    free(firstLines);

    MPI_Finalize();
    return 0;
}
