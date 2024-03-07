#include <float.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const double EPSILON = 0.0000001;
const double TAU = 0.000001;
const int MAX_ITERATIONS = 100000;

void Init(double *A, double *B, const int n) {
    for (int i = 0; i < n; ++i) {
        B[i] = n + 1;
        for (int j = 0; j < n; ++j) {
            A[i * n + j] = 1 + (i == j);
        }
    }
}

double calcEndValue(const double *bVector, int n, double eps) {
    double res = 0;
#pragma omp parallel for reduction(+ : res)
    for (int i = 0; i < n; ++i) {
        res += bVector[i] * bVector[i];
    }
    return res * eps * eps;
}

int solve(const double *A, const double *b, double *x, double *x_n, int n,
          double bLen, double tau) {
    int countIters = 0;
    double nextParam;
    double prevParam = DBL_MAX;
    int flag = 1;
    for (; flag && (countIters < MAX_ITERATIONS); ++countIters) {
        nextParam = 0;
#pragma omp parallel for reduction(+ : nextParam)
        for (int i = 0; i < n; ++i) {
            double sum = -b[i];
#pragma omp parallel for reduction(+ : sum)
            for (int j = 0; j < n; ++j) {
                sum += A[i * n + j] * x_n[j];
            }
            nextParam += sum * sum;    // reduce
            x[i] = x_n[i] - sum * tau;
        }
        if (prevParam <= nextParam) {
            flag = 0;
        }
        memcpy(x_n, x, n * sizeof(double));    // swap
        if (nextParam < bLen || nextParam == DBL_MAX) {
            break;
        }
        prevParam = nextParam;
    }
    return countIters;
}

int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    int countThreads = atoi(argv[2]);

    double *A = (double *)malloc(n * n * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    double *x_n = (double *)malloc(n * sizeof(double));
    double *x = (double *)calloc(n, sizeof(double));
    Init(A, b, n);

    omp_set_num_threads(countThreads);

    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    double bLen = calcEndValue(b, n, EPSILON);

    int countIters = solve(A, b, x, x_n, n, bLen, TAU);

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
