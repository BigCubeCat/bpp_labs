project(Core)

include_directories(pi)
include_directories(TaskList)
include_directories(Storage)

set(SOURCE_LIB Core.h Core.cpp)

add_library(Core ${SOURCE_LIB})

add_subdirectory(pi)
add_subdirectory(TaskList)
add_subdirectory(Storage)

target_link_libraries(Core TaskList Storage pi)
