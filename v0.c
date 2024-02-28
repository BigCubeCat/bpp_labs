#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EPSILON 0.0000001
#define TAU 0.000001
#define MAX_ITERATIONS 100000
#define N 1000
//
// void print(double *x, const int n) {
//     for (int i = 0; i < n; ++i) {
//         printf("%.3f ", x[i]);
//     }
//     printf("\n");
// }
//
void InitA(double *A, const int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i * n + j] = 1 + (i == j);
        }
    }
}

void InitB(double *B, const int n) {
    for (int i = 0; i < n; ++i) {
        B[i] = n + 1;
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    double *A = (double *)malloc(N * N * sizeof(double));
    double *b = (double *)malloc(N * sizeof(double));
    double *x_n = (double *)malloc(N * sizeof(double));
    double *x = (double *)calloc(N, sizeof(double));

    InitA(A, N);
    InitB(b, N);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    int countIters = 0;
    double endParam = DBL_MAX;
    double prevParam = DBL_MAX;

    // length of B vector
    double EPSILON_SQUARE = 0;
    for (int i = 0; i < N; ++i) EPSILON_SQUARE += b[i] * b[i];
    EPSILON_SQUARE *= EPSILON * EPSILON;

    // TAU setup
    int useTau = 0;
    double tau = TAU;

    for (; countIters < MAX_ITERATIONS; ++countIters) {
        endParam = 0;
        for (int i = 0; i < N; ++i) {
            double sum = -b[i];    // сразу отнимаем b
            for (int j = 0; j < N; ++j) {
                sum += A[i * N + j] * x_n[j];
            }
            x[i] = x_n[i] - sum * tau;
            endParam += sum * sum;
        }
        if (prevParam <= endParam) {    // условие смены знака скаляра.
            if (useTau)
                break;    // Очевидно, что на прямой к числу можно приближаться
                          // либо слева, либо справа
            tau *= -1;
            useTau = 1;
        }
        memcpy(x_n, x, N * sizeof(double));    // swap
        if (endParam < EPSILON_SQUARE ||
            endParam == DBL_MAX) {    // Условия выхода из спецификации
            break;
        }
        prevParam = endParam;
    }
    printf("%f\n", x[0]);
    printf("count iterations = %d\n", countIters);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Time taken: %lf sec.\n",
           end.tv_sec - start.tv_sec +
               0.000000001 * (end.tv_nsec - start.tv_nsec));

    free(A);
    free(b);
    free(x_n);
    free(x);

    return 0;
}
