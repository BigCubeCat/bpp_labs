cmake_minimum_required(VERSION 3.2)

project(matmult)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -fopenmp -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")


add_executable(matmult src/main.cpp
        src/MatrixModel/MatrixModel.h
        src/FileWorker/FileWorker.h
        src/FileWorker/FileWorker.cpp
        src/MatrixModel/MatrixModel.cpp
        src/utils/utils.h
        src/utils/utils.cpp
        src/algo/algo.h
        src/algo/algo.cpp
        src/runner/runner.h
        src/runner/runner.cpp
        src/ConfigStruct/TConfigStruct.h
        src/ConfigStruct/TConfigStruct.cpp
)

# MPI
find_package(MPI REQUIRED)
target_link_libraries(matmult PUBLIC MPI::MPI_CXX)
