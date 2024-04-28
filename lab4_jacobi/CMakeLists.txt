cmake_minimum_required(VERSION 3.20)
project(Jacobi)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -fopenmp -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(Jacobi src/main.cpp
        src/Conf/ConfReader.h
        src/Conf/ConfReader.cpp
        src/Algo/Algo.h
        src/Algo/Algo.cpp
        src/Algo/Vector3.h
        src/Algo/Vector3.cpp
        src/Util/util.cpp)


# MPI
find_package(MPI REQUIRED)
target_link_libraries(Jacobi PUBLIC MPI::MPI_CXX)
