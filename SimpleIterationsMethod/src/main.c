#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <malloc.h>

#include "parser/parser.h"
#include "CtxData/CtxData.h"
#include "calculation/calculation.h"
#include "solve/solve.h"

const int MAX_ITERATIONS = 10000;
const double EPSILON = 0.000001;
const double TAU = 0.00001;
const size_t N = 2500;


#ifdef USE_MPE
#include <mpe.h>
#endif


void printUsage() {
    printf("Usage: \n");
    printf("mpiexec -np <count MPI process> SimpleIterationsMethod <filename>\n");
}

int argparse(int argc, char *argv[], CtxData *data) {
    if (argc != 2) {
        printf("no input file; generating data for n = %ld\n", N);
        generateData(data, N);
        return 0;
    } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printUsage();
        return 1;
    }
    generateData(data, N);
    return 0;
}

int main(int argc, char *argv[]) {
    int size, rank;
    MPI_Init(&argc, &argv);
#ifdef USE_MATH_LIB
    MPE_Init_log();
#endif
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    CtxData data;
    data.n = N;

    if (rank == 0 && argparse(argc, argv, &data)) {
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }
    double startTime = MPI_Wtime();
    prepare(&data, size);
    printf("data.n = %zd\n", data.n);
    size_t currentSize = (rank == 0 ? data.n : (size_t) data.linesCount[rank]);

    if (rank != 0) {
        /* выделение памяти */
        data.matrix = (double *) malloc(sizeof(double) * data.n * currentSize);
        data.b_vector = (double *) malloc(sizeof(double) * currentSize);
        data.x_vector = (double *) malloc(sizeof(double) * data.n);
        data.x_new_vector = (double *) calloc(data.linesCount[rank], sizeof(double));
    }
    syncStartData(&data, rank, size);
    if (rank == 0) bLenCalc(&data, EPSILON);

    int countIter = solve(&data, TAU, rank, MAX_ITERATIONS);

    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;
    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
        printf("%f\n", maxTime);
        printf("\n--------------\n");
        printf("\n%f\n", data.x_new_vector[0]);
        printf("count iterations = %d\n", countIter);
        printf("Count of MPI process: %d\n", size);
        printf("\n--------------\n");
    }

    freeCtx(&data);
    MPI_Finalize();
    return 0;

}
