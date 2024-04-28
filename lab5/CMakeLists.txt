cmake_minimum_required(VERSION 3.29)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/Core)

add_executable(lab5 src/main.cpp)

add_subdirectory(src/Core)

target_link_libraries(lab5 Core)
