cmake_minimum_required(VERSION 3.29)
project(Jacobi)

set(CMAKE_CXX_STANDARD 17)

add_executable(Jacobi src/main.cpp
        src/Conf/ConfReader.h
        src/Conf/ConfReader.cpp
        src/Algo/Algo.h
        src/Algo/Algo.cpp
        src/Algo/Vector3.h
        src/Algo/Vector3.cpp)
