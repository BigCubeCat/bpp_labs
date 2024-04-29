cmake_minimum_required(VERSION 3.20)
project(lab5)


set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -fopenmp -ftree-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(CMAKE_CXX_STANDARD 17)

include_directories(src/Core)
include_directories(src/Config)
include_directories(src/Storage)
include_directories(src/TaskList)

add_executable(
        lab5
        src/main.cpp
        src/Worker/Worker.h
        src/Worker/Worker.cpp
        src/Worker/EStatus.h
)

add_subdirectory(src/Core)
add_subdirectory(src/Config)
add_subdirectory(src/Storage)
add_subdirectory(src/TaskList)

find_package(MPI REQUIRED)
target_link_libraries(lab5 Core Config Storage TaskList MPI::MPI_CXX)
