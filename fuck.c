
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double calculate(double *matrix, double *x, double *b, double *new_x, int n,
                 double tao, int cnt, int first) {

    double res = 0;

    for (int i = 0; i < cnt; ++i) {
        double s = -b[i];
        for (int j = 0; j < n; ++j) {
            s += matrix[i * n + j] * x[j];
        }

        res += s * s;
        new_x[i] = x[first + i] - tao * s;
    }

    return res;
}

int countOfLines(int n, int rank, int size) {
    return n / size + (rank < n % size);
}

int first_line(int n, int rank, int size) {

    int res = 0;
    for (int i = 0; i < rank; ++i) {
        res += countOfLines(n, i, size);
    }

    return res;
}

void init(double *matrix, double *b, double *x, int n) {

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i * n + j] = 1;
        }
    }

    for (int i = 0; i < n; ++i) {
        x[i] = 0;
        b[i] = n + 1;
        matrix[i * n + i] = 2;
    }
}

#define MESSAGE_TAG 666

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n;
    double eps;
    double tao;

    if (rank == 0) {
        n = 100;
        eps = 0.000001;
        tao = 0.001;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tao, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int *linesCount = (int *) malloc(size * sizeof(int));
    int *firstLines = (int *) malloc(size * sizeof(int));
    firstLines[0] = 0;

    for (int i = 0; i < size; ++i) {
        linesCount[i] = countOfLines(n, i, size);
        if (i > 0)
            firstLines[i] = firstLines[i - 1] + linesCount[i - 1];

        printf("linesCount[%d] = %d\n", i, linesCount[i]);
        printf("firstLines[%d] = %d\n", i, firstLines[i]);

    }

    double *matrix = (double *) malloc(sizeof(double) * n *
                                       (rank == 0 ? n : linesCount[rank]));
    double *x = (double *) malloc(sizeof(double) * n);
    double *b =
            (double *) malloc(sizeof(double) * (rank == 0 ? n : linesCount[rank]));

    int buff_size = sizeof(double) * (linesCount[0] * n + n + linesCount[0]);
    char *buff = (char *) malloc(buff_size);

    if (rank == 0) {
        init(matrix, b, x, n);

        for (int i = 1; i < size; ++i) {

            int pos = 0;
            MPI_Pack(matrix + n * firstLines[i], n * linesCount[i], MPI_DOUBLE,
                     buff, buff_size, &pos, MPI_COMM_WORLD);
            MPI_Pack(x, n, MPI_DOUBLE, buff, buff_size, &pos, MPI_COMM_WORLD);
            MPI_Pack(b + firstLines[i], linesCount[i], MPI_DOUBLE, buff, buff_size,
                     &pos, MPI_COMM_WORLD);

            MPI_Send(buff, buff_size, MPI_BYTE, i, MESSAGE_TAG, MPI_COMM_WORLD);
        }
    } else {

        MPI_Recv(buff, buff_size, MPI_BYTE, 0, MESSAGE_TAG, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        int pos = 0;
        MPI_Unpack(buff, buff_size, &pos, matrix, n * linesCount[rank], MPI_DOUBLE,
                   MPI_COMM_WORLD);
        MPI_Unpack(buff, buff_size, &pos, x, n, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(buff, buff_size, &pos, b, linesCount[rank], MPI_DOUBLE,
                   MPI_COMM_WORLD);
    }
    free(buff);

    double b_norm_sqr = 0;
    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            b_norm_sqr += b[i] * b[i];
        }
    }

    double *new_x = (double *) malloc(sizeof(double) * linesCount[rank]);
    double *d;
    if (rank == 0) {
        d = (double *) malloc(sizeof(double) * size);
    }

    int flag = 1;
    while (flag) {

        double dd = calculate(matrix, x, b, new_x, n, tao, linesCount[rank],
                              firstLines[rank]);

        MPI_Allgatherv(new_x, linesCount[rank], MPI_DOUBLE, x, linesCount,
                       firstLines, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Gather(&dd, 1, MPI_DOUBLE, d, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            double s = 0;
            for (int i = 0; i < size; ++i) {
                s += d[i];
            }
            flag = sqrt(s / b_norm_sqr) > eps;
        }

        MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            printf("%.3f ", x[i]);
        }
        printf("\n");
    }

    if (rank == 0) {
        fprintf(stderr, "\n============================\n");
        fprintf(stderr, "Count of MPI process: %d\n", size);
    }
    free(matrix);
    free(x);
    free(new_x);
    free(b);
    if (rank == 0) {
        free(d);
    }
    free(linesCount);
    free(firstLines);

    MPI_Finalize();
    return 0;
}

