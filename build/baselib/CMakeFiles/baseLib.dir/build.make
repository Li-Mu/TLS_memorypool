# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build

# Include any dependencies generated for this target.
include baselib/CMakeFiles/baseLib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include baselib/CMakeFiles/baseLib.dir/compiler_depend.make

# Include the progress variables for this target.
include baselib/CMakeFiles/baseLib.dir/progress.make

# Include the compile flags for this target's objects.
include baselib/CMakeFiles/baseLib.dir/flags.make

baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o: baselib/CMakeFiles/baseLib.dir/flags.make
baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o: ../baselib/CentralCache.cpp
baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o: baselib/CMakeFiles/baseLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o -MF CMakeFiles/baseLib.dir/CentralCache.cpp.o.d -o CMakeFiles/baseLib.dir/CentralCache.cpp.o -c /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/CentralCache.cpp

baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/baseLib.dir/CentralCache.cpp.i"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/CentralCache.cpp > CMakeFiles/baseLib.dir/CentralCache.cpp.i

baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/baseLib.dir/CentralCache.cpp.s"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/CentralCache.cpp -o CMakeFiles/baseLib.dir/CentralCache.cpp.s

baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o: baselib/CMakeFiles/baseLib.dir/flags.make
baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o: ../baselib/PageCache.cpp
baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o: baselib/CMakeFiles/baseLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o -MF CMakeFiles/baseLib.dir/PageCache.cpp.o.d -o CMakeFiles/baseLib.dir/PageCache.cpp.o -c /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/PageCache.cpp

baselib/CMakeFiles/baseLib.dir/PageCache.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/baseLib.dir/PageCache.cpp.i"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/PageCache.cpp > CMakeFiles/baseLib.dir/PageCache.cpp.i

baselib/CMakeFiles/baseLib.dir/PageCache.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/baseLib.dir/PageCache.cpp.s"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/PageCache.cpp -o CMakeFiles/baseLib.dir/PageCache.cpp.s

baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o: baselib/CMakeFiles/baseLib.dir/flags.make
baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o: ../baselib/ThreadCache.cpp
baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o: baselib/CMakeFiles/baseLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o -MF CMakeFiles/baseLib.dir/ThreadCache.cpp.o.d -o CMakeFiles/baseLib.dir/ThreadCache.cpp.o -c /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/ThreadCache.cpp

baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/baseLib.dir/ThreadCache.cpp.i"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/ThreadCache.cpp > CMakeFiles/baseLib.dir/ThreadCache.cpp.i

baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/baseLib.dir/ThreadCache.cpp.s"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib/ThreadCache.cpp -o CMakeFiles/baseLib.dir/ThreadCache.cpp.s

# Object files for target baseLib
baseLib_OBJECTS = \
"CMakeFiles/baseLib.dir/CentralCache.cpp.o" \
"CMakeFiles/baseLib.dir/PageCache.cpp.o" \
"CMakeFiles/baseLib.dir/ThreadCache.cpp.o"

# External object files for target baseLib
baseLib_EXTERNAL_OBJECTS =

baselib/libbaseLib.a: baselib/CMakeFiles/baseLib.dir/CentralCache.cpp.o
baselib/libbaseLib.a: baselib/CMakeFiles/baseLib.dir/PageCache.cpp.o
baselib/libbaseLib.a: baselib/CMakeFiles/baseLib.dir/ThreadCache.cpp.o
baselib/libbaseLib.a: baselib/CMakeFiles/baseLib.dir/build.make
baselib/libbaseLib.a: baselib/CMakeFiles/baseLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libbaseLib.a"
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && $(CMAKE_COMMAND) -P CMakeFiles/baseLib.dir/cmake_clean_target.cmake
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/baseLib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
baselib/CMakeFiles/baseLib.dir/build: baselib/libbaseLib.a
.PHONY : baselib/CMakeFiles/baseLib.dir/build

baselib/CMakeFiles/baseLib.dir/clean:
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib && $(CMAKE_COMMAND) -P CMakeFiles/baseLib.dir/cmake_clean.cmake
.PHONY : baselib/CMakeFiles/baseLib.dir/clean

baselib/CMakeFiles/baseLib.dir/depend:
	cd /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/baselib /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib /home/ihavc01/data/ihavc01/LPT/TLS_MemoryPool-master/ConcurrentMemoryPool/build/baselib/CMakeFiles/baseLib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : baselib/CMakeFiles/baseLib.dir/depend
