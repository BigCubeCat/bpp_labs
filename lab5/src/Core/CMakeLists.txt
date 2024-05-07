project(Core)

include_directories(pi)

set(SOURCE_LIB Core.h Core.cpp)

add_library(Core ${SOURCE_LIB})

add_subdirectory(pi)
add_subdirectory(TaskList)

target_link_libraries(Core TaskList pi)
