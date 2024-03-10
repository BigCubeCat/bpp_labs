cmake_minimum_required(VERSION 3.28)
project(SimpleIterationsMethod C)

set(CMAKE_C_STANDARD 11)

# -fopenmp flag (enables OpenMP)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3")
# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(SimpleIterationsMethod src/main.c
        src/parser/parser.h
        src/parser/parser.c
        src/calculation/calculation.h
        src/calculation/calculation.c
        src/CtxData/CtxData.h
        src/CtxData/CtxData.c
        src/solve/solve.h
        src/solve/solve.c
)

# OpenMP
find_package(OpenMP)
if (OpenMP_C_FOUND)
    target_link_libraries(SimpleIterationsMethod PUBLIC OpenMP::OpenMP_C)
endif ()

# MPI
find_package(MPI REQUIRED)
target_link_libraries(SimpleIterationsMethod PUBLIC MPI::MPI_C)


