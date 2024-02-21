#include <malloc.h>
#include <mpi.h>
#include <stdio.h>

#define EPSILON 0.0000001
#define TAU 0.000001
#define MAX_ITERATIONS 100000
#define N 11

double *genA(const int n) {
  double *res = (double *)malloc(n * n * sizeof(double));
  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < n; ++col) {
      res[row * n + col] = (row == col) ? 2 : 1;
    }
  }
  return res;
}

double *genB(const int n) {
  double *res = (double *)malloc(n * sizeof(double));
  for (int col = 0; col < n; ++col) {
    res[col] = n + 1;
  }
  return res;
}

double *genX(const int n) { return (double *)calloc(n, sizeof(double)); }

void mulMatOnVec(double *matrix, double *vector, double *result,
                 const int countRows, const int countCols, const int gap) {
  for (int row = 0; row < countRows; ++row) {
    for (int col = 0; col < countCols; ++col) {
      result[row] += matrix[row * countCols + col + gap] * vector[col];
    }
  }
}

int isLargePart(const int rank, const int count) { return rank < count; }

int lineGaps(const int n, const int basicSize, const int largeSize,
             const int countLarge) {
  if (n < countLarge) {
    return largeSize * n;
  }
  return largeSize * countLarge + (n - countLarge) * basicSize;
}

int main(int argc, char *argv[]) {
  int procCount, procRank, basicSize, largeSize, countLarge, isLarge,
      currentSize;
  double *aMatrix;
  double *bVector;
  double *xVector;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  basicSize = N / procCount;
  countLarge = N % procCount;
  largeSize = basicSize + (countLarge == 0 ? 0 : 1);
  isLarge = isLargePart(procCount, largeSize);
  currentSize = (isLarge) ? largeSize : basicSize;

  if (procRank == 0) {
    aMatrix = genA(N);
    bVector = genB(N);
    xVector = genX(N);
  }

  if (procRank == 0) {
    int *lines = (int *)malloc(procCount * sizeof(int));
    int *procSizes = (int *)malloc(procCount * sizeof(int));
    for (int i = 1; i < procCount; ++i) {
      lines[i] = lineGaps(i, basicSize, largeSize, countLarge);
      procSizes[i] = isLargePart(i, countLarge);

      printf("%d skip lines for %d = %d\n", procRank, i, lines[i]);
      printf("%d proc Sizes for %d = %d\n", procRank, i, procSizes[i]);

      MPI_Send(aMatrix + N * lines[i], procSizes[i], MPI_DOUBLE, i, 100,
               MPI_COMM_WORLD);
      MPI_Send(xVector + lines[i], procSizes[i], MPI_DOUBLE, i, 101,
               MPI_COMM_WORLD);
      MPI_Send(bVector, largeSize, MPI_DOUBLE, i, 10, MPI_COMM_WORLD);
    }
    for (int pId = 0; pId < procCount; ++pId) {
    }
  }

  MPI_Finalize();
  return 0;
}
