# Решение СЛАУ методом простой итерации
v0 - без MPI
v1 - с MPI
v2 - с MPI и разрезанием вектора между процессами (с последующей передачей по кольцу) 

## Build
```bash
mpicc v0.c
```

## Run
```bash
mpirun -np <count process> <exe>
```