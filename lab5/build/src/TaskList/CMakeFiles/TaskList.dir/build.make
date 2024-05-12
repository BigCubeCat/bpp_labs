# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build

# Include any dependencies generated for this target.
include src/TaskList/CMakeFiles/TaskList.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/TaskList/CMakeFiles/TaskList.dir/compiler_depend.make

# Include the progress variables for this target.
include src/TaskList/CMakeFiles/TaskList.dir/progress.make

# Include the compile flags for this target's objects.
include src/TaskList/CMakeFiles/TaskList.dir/flags.make

src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o: src/TaskList/CMakeFiles/TaskList.dir/flags.make
src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o: /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/src/TaskList/TaskList.cpp
src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o: src/TaskList/CMakeFiles/TaskList.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o"
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o -MF CMakeFiles/TaskList.dir/TaskList.cpp.o.d -o CMakeFiles/TaskList.dir/TaskList.cpp.o -c /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/src/TaskList/TaskList.cpp

src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TaskList.dir/TaskList.cpp.i"
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/src/TaskList/TaskList.cpp > CMakeFiles/TaskList.dir/TaskList.cpp.i

src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TaskList.dir/TaskList.cpp.s"
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/src/TaskList/TaskList.cpp -o CMakeFiles/TaskList.dir/TaskList.cpp.s

# Object files for target TaskList
TaskList_OBJECTS = \
"CMakeFiles/TaskList.dir/TaskList.cpp.o"

# External object files for target TaskList
TaskList_EXTERNAL_OBJECTS =

src/TaskList/libTaskList.a: src/TaskList/CMakeFiles/TaskList.dir/TaskList.cpp.o
src/TaskList/libTaskList.a: src/TaskList/CMakeFiles/TaskList.dir/build.make
src/TaskList/libTaskList.a: src/TaskList/CMakeFiles/TaskList.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libTaskList.a"
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && $(CMAKE_COMMAND) -P CMakeFiles/TaskList.dir/cmake_clean_target.cmake
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TaskList.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/TaskList/CMakeFiles/TaskList.dir/build: src/TaskList/libTaskList.a
.PHONY : src/TaskList/CMakeFiles/TaskList.dir/build

src/TaskList/CMakeFiles/TaskList.dir/clean:
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList && $(CMAKE_COMMAND) -P CMakeFiles/TaskList.dir/cmake_clean.cmake
.PHONY : src/TaskList/CMakeFiles/TaskList.dir/clean

src/TaskList/CMakeFiles/TaskList.dir/depend:
	cd /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5 /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/src/TaskList /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList /home/bigcubecat/Projects/NSU/sem4/pp/bpp_labs/lab5/build/src/TaskList/CMakeFiles/TaskList.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/TaskList/CMakeFiles/TaskList.dir/depend

