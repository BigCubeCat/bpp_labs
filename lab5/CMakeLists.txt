cmake_minimum_required(VERSION 3.20)
project(lab5)


set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -fopenmp -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(CMAKE_CXX_STANDARD 17)

include_directories(src/Core)
include_directories(src/Config)

add_executable(
        lab5
        src/main.cpp
        src/Worker/Worker.h
        src/Worker/Worker.cpp
        src/Worker/EStatus.h
        src/Worker/Profiler.h
        src/Worker/Profiler.cpp
        src/Worker/MutualMem.h
)

add_subdirectory(src/Core)
add_subdirectory(src/Config)

find_package(MPI REQUIRED)
target_link_libraries(lab5 Core Config MPI::MPI_CXX)
