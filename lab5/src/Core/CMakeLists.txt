project(Core)

include_directories(pi)

set(SOURCE_LIB Core.h Core.cpp)

add_library(Core ${SOURCE_LIB})

add_subdirectory(pi)

target_link_libraries(Core pi)
