cmake_minimum_required(VERSION 3.2)

project(matmult)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")


# Use imported targets would make things much eazier. Thanks Levi for pointing it out.
add_executable(matmult src/main.cpp
        src/MatrixModel/MatrixModel.h
        src/FileWorker/FileWorker.h
        src/FileWorker/FileWorker.cpp
        src/MatrixModel/MatrixModel.cpp
        src/utils/utils.h
        src/utils/utils.cpp
        src/CalculationController/CalculationController.h
        src/CalculationController/CalculationController.cpp)


# MPI
find_package(MPI REQUIRED)
target_link_libraries(matmult PUBLIC MPI::MPI_CXX)
