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
        src/runner/runner.h
        src/runner/runner.cpp
        src/ConfigStruct/TConfigStruct.h
        src/ConfigStruct/TConfigStruct.cpp
        src/runner/setup.h
        src/runner/setup.cpp
)

# MPI
find_package(MPI REQUIRED)
target_link_libraries(matmult PUBLIC MPI::MPI_CXX)

# MKL
#set(MKL_ARCH intel64)
#set(MKL_THREADING sequential)
#set(MKL_INTERFACE lp64)
#find_package(MKL CONFIG REQUIRED PATHS $ENV{MKLROOT})
#message(STATUS "Imported oneMKL targets: ${MKL_IMPORTED_TARGETS}")
#
#target_compile_options(matmult PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_COMPILE_OPTIONS>)
#target_include_directories(matmult PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_INCLUDE_DIRECTORIES>)
#target_link_libraries(matmult PUBLIC $<LINK_ONLY:MKL::MKL>)
