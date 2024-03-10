#include <mpi.h>
#include <float.h>
#include "./solve.h"

double calc(CtxData *data, double tao, int rank) {
    double res = 0;
#pragma omp parallel for reduction(+ : res)
    for (size_t i = 0; i < (size_t) data->linesCount[rank]; ++i) {
        double s = -data->b_vector[i];
#pragma omp parallel for reduction(+ : s)
        for (size_t j = 0; j < data->n; ++j) {
            s += data->matrix[i * data->n + j] * data->x_vector[j];
        }
        res += s * s;
        data->x_new_vector[i] = data->x_vector[(size_t) data->linesCount[rank] + i] - tao * s;
    }
    return res;
}


double sumVector(const double *vector, int size) {
    double result = 0;
    for (int i = 0; i < size; ++i) {
        result += vector[i];
    }
    return result;
}

int solve(CtxData *data, double tao, int rank, size_t max_iterations) {
    int countIter = 0;
    int flag = 1;
    double prev = DBL_MAX;
    double next;

    for (; flag && (countIter < max_iterations); ++countIter) {
        double dd = calc(data, tao, rank);
        MPI_Allgatherv(data->x_new_vector, data->linesCount[rank], MPI_DOUBLE, data->x_vector,
                       data->linesCount, data->firstLines, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allreduce(&dd, &next, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        flag = (prev >= next) && (next >= data->b_length);
        prev = next;
    }
    return countIter;
}
