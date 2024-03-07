#include <float.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const double EPSILON = 0.0000001;
const double TAU = 0.000001;

void Init(double *A, double *B, const int n) {
    for (int i = 0; i < n; ++i) {
        B[i] = n + 1;
        for (int j = 0; j < n; ++j) {
            A[i * n + j] = 1 + (i == j);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        perror("Usage: ./exe <N> <num threads>");
        return 1;
    }
    int n = atoi(argv[1]);
    int countThreads = atoi(argv[2]);

    double *A = (double *)malloc(n * n * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    double *x_n = (double *)malloc(n * sizeof(double));
    double *x = (double *)calloc(n, sizeof(double));
    double bLen = 0;
    Init(A, b, n);

    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    int countIters = 0;
    double nextParam = DBL_MAX;
    double prevParam = DBL_MAX;
    int flag = 1;
    double tau = TAU;
    int useTau = 0;

    omp_set_num_threads(countThreads);

#pragma omp parallel
    {
#pragma omp for schedule(static) reduction(+ : bLen)
        for (int i = 0; i < n; ++i) {
            bLen += b[i] * b[i];
        }
        bLen *= EPSILON * EPSILON;

        while (flag) {
            nextParam = 0;
#pragma omp for schedule(static) reduction(+ : nextParam)
            for (int i = 0; i < n; ++i) {
                double sum = -b[i];
                for (int j = 0; j < n; ++j) {
                    sum += A[i * n + j] * x_n[j];
                }
                x[i] = x_n[i] - sum * tau;
#pragma omp atomic
                nextParam += sum * sum;
            }
#pragma omp single
            {
                ++countIters;
                if (prevParam <= nextParam) {    // условие смены знака скаляра.
                    tau *= -1;
                    if (useTau) {
                        flag = 0;
                    }
                    useTau = 1;
                }
                memcpy(x_n, x, n * sizeof(double));    // swap
                if (nextParam < bLen) {
                    flag = 0;
                }
                prevParam = nextParam;
            }
        }
    }

    ftime = omp_get_wtime();
    exec_time = ftime - itime;
    printf("%f\n", exec_time);
    printf("%d\n", countIters);
    printf("%f\n", x[0]);

    free(A);
    free(b);
    free(x_n);
    free(x);

    return 0;
}
