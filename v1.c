#include <float.h>
#include <mpe.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_ITERATIONS = 10000;
const double EPSILON = 0.000001;
const double TAU = 0.00001;
const int N = 10000;

double calc(double *matrix, double *xVector, double *bVector,
            double *xVectorNew, int n, double tao, int cnt, int first) {
    double res = 0;
    for (int i = 0; i < cnt; ++i) {
        double s = -bVector[i];
        for (int j = 0; j < n; ++j) {
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

int main(int argc, char **argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPE_Init_log();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double startTime = MPI_Wtime();

    /* Считаем конфигурацию себя и других */
    int *linesCount = (int *)malloc(size * sizeof(int));
    int *firstLines = (int *)malloc(size * sizeof(int));
    initLinesSettings(linesCount, firstLines, size, N);
    int currentSize = (rank == 0 ? N : linesCount[rank]);
    /* выделение памяти */
    double *matrix = (double *)malloc(sizeof(double) * N * currentSize);
    double *xVector = (double *)malloc(sizeof(double) * N);
    double *bVector = (double *)malloc(sizeof(double) * currentSize);
    double *xVectorNew = (double *)calloc(linesCount[rank], sizeof(double));
    int buffSize = sizeof(double) * (linesCount[0] * N + N + linesCount[0]);
    void *buff = (void *)malloc(buffSize);

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

    double bLen = (rank == 0) ? calcEndValue(bVector, N, EPSILON) : 0;

    double *d = (double *)malloc(sizeof(double) * ((rank == 0) ? size : 1));

    int flag = 1;
    int useTau = 0;
    double tau = TAU;
    double prevParam = DBL_MAX;
    int countIter = 0;

    for (; flag && (countIter < MAX_ITERATIONS); ++countIter) {
        double dd = calc(matrix, xVector, bVector, xVectorNew, N, tau,
                         linesCount[rank], firstLines[rank]);
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
                }
            }
            prevParam = endParam;
            flag = flag && endParam > bLen;
        }
        MPI_Bcast(&tau, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    /*
    if (rank == 0) {
        printf("\n--------------\n");
        printf("\n%f\n", xVectorNew[0]);
        printf("count iterations = %d\n", countIter);
        printf("Count of MPI process: %d\n", size);
        printf("\n--------------\n");
    }
  */
    free(matrix);
    free(xVector);
    free(xVectorNew);
    free(bVector);
    free(d);
    free(linesCount);
    free(firstLines);

    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;

    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) printf("%f\n", maxTime);

    MPI_Finalize();
    return 0;
}
