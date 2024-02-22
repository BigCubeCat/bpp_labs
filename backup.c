#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EPSILON 0.0000001
#define TAU 0.000001
#define MAX_ITERATIONS 100000
#define N 100

void print(double *x, const int n) {
    for (int i = 0; i < n; ++i) {
        printf("%.3f ", x[i]);
    }
    printf("\n");
}

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

void mulVecScal(const double *vec, double *res, const double lambda) {
    for (int i = 0; i < N; ++i) {
        res[i] = vec[i] * lambda;
    }
}

void subVec(const double *first, const double *second, double *result) {
    for (int i = 0; i < N; ++i) {
        result[i] = first[i] - second[i];
    }
}

void mulMatVec(const double *mat, const double *vec, double *result) {
    memset(result, 0, N * sizeof(double));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i] += mat[i * N + j] * vec[j];
        }
    }
}

double lengthSquare(const double *vec) {
    double sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += vec[i] * vec[i];
    }
    return sum;
}

int solve(double *x_n, const double *A, const double *b, double *x) {
    int countIters = 0;

    double endParam;
    double prevParam = DBL_MAX;

    double EPSILON_SQUARE = EPSILON * EPSILON * lengthSquare(b);

    int useTau = 0;
    double tau = TAU;

    for (; countIters < MAX_ITERATIONS; ++countIters) {
        mulMatVec(A, x_n, x); // Ax
        subVec(x, b, x);      // Ax - b
        endParam = lengthSquare(x);
        if (prevParam <= endParam) {
            if (useTau)
                break;
            tau *= -1;
            useTau = 1;
        }

        mulVecScal(x, x, tau); // τ(Ax - b)
        subVec(x_n, x, x);     // x^(n+1) = x^n - τ(Ax - b)
        memcpy(x_n, x, N * sizeof(double));

        if (endParam < EPSILON_SQUARE || endParam == DBL_MAX) {
            break;
        }
        prevParam = endParam;
    }
    return countIters;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    double *A = (double *) malloc(N * N * sizeof(double));
    double *B = (double *) malloc(N * sizeof(double));
    double *X0 = (double *) malloc(N * sizeof(double));
    double *X = (double *) calloc(N, sizeof(double));

    InitA(A, N);
    InitB(B, N);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    printf("%d\n", solve(X0, A, B, X));
    print(X, N);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Time taken: %lf sec.\n",
           end.tv_sec - start.tv_sec +
           0.000000001 * (end.tv_nsec - start.tv_nsec));

    free(A);
    free(B);
    free(X0);
    free(X);

    return 0;
}
