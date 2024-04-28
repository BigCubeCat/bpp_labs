cmake_minimum_required(VERSION 3.20)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/Core)
include_directories(src/Config)
include_directories(src/Storage)

add_executable(lab5 src/main.cpp)

add_subdirectory(src/Core)
add_subdirectory(src/Config)
add_subdirectory(src/Storage)

target_link_libraries(lab5 Core Config Storage)
