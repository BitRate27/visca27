# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.30.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.30.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/livestreammedia/visca27

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/livestreammedia/visca27/build

# Include any dependencies generated for this target.
include CMakeFiles/visca27.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/visca27.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/visca27.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/visca27.dir/flags.make

CMakeFiles/visca27.dir/visca27.cpp.o: CMakeFiles/visca27.dir/flags.make
CMakeFiles/visca27.dir/visca27.cpp.o: /Users/livestreammedia/visca27/visca27.cpp
CMakeFiles/visca27.dir/visca27.cpp.o: CMakeFiles/visca27.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/livestreammedia/visca27/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/visca27.dir/visca27.cpp.o"
	/usr/local/bin/ccache /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/visca27.dir/visca27.cpp.o -MF CMakeFiles/visca27.dir/visca27.cpp.o.d -o CMakeFiles/visca27.dir/visca27.cpp.o -c /Users/livestreammedia/visca27/visca27.cpp

CMakeFiles/visca27.dir/visca27.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/visca27.dir/visca27.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/livestreammedia/visca27/visca27.cpp > CMakeFiles/visca27.dir/visca27.cpp.i

CMakeFiles/visca27.dir/visca27.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/visca27.dir/visca27.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/livestreammedia/visca27/visca27.cpp -o CMakeFiles/visca27.dir/visca27.cpp.s

CMakeFiles/visca27.dir/ViscaAPI.cpp.o: CMakeFiles/visca27.dir/flags.make
CMakeFiles/visca27.dir/ViscaAPI.cpp.o: /Users/livestreammedia/visca27/ViscaAPI.cpp
CMakeFiles/visca27.dir/ViscaAPI.cpp.o: CMakeFiles/visca27.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/livestreammedia/visca27/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/visca27.dir/ViscaAPI.cpp.o"
	/usr/local/bin/ccache /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/visca27.dir/ViscaAPI.cpp.o -MF CMakeFiles/visca27.dir/ViscaAPI.cpp.o.d -o CMakeFiles/visca27.dir/ViscaAPI.cpp.o -c /Users/livestreammedia/visca27/ViscaAPI.cpp

CMakeFiles/visca27.dir/ViscaAPI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/visca27.dir/ViscaAPI.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/livestreammedia/visca27/ViscaAPI.cpp > CMakeFiles/visca27.dir/ViscaAPI.cpp.i

CMakeFiles/visca27.dir/ViscaAPI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/visca27.dir/ViscaAPI.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/livestreammedia/visca27/ViscaAPI.cpp -o CMakeFiles/visca27.dir/ViscaAPI.cpp.s

# Object files for target visca27
visca27_OBJECTS = \
"CMakeFiles/visca27.dir/visca27.cpp.o" \
"CMakeFiles/visca27.dir/ViscaAPI.cpp.o"

# External object files for target visca27
visca27_EXTERNAL_OBJECTS =

libvisca27.a: CMakeFiles/visca27.dir/visca27.cpp.o
libvisca27.a: CMakeFiles/visca27.dir/ViscaAPI.cpp.o
libvisca27.a: CMakeFiles/visca27.dir/build.make
libvisca27.a: CMakeFiles/visca27.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/livestreammedia/visca27/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libvisca27.a"
	$(CMAKE_COMMAND) -P CMakeFiles/visca27.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/visca27.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/visca27.dir/build: libvisca27.a
.PHONY : CMakeFiles/visca27.dir/build

CMakeFiles/visca27.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/visca27.dir/cmake_clean.cmake
.PHONY : CMakeFiles/visca27.dir/clean

CMakeFiles/visca27.dir/depend:
	cd /Users/livestreammedia/visca27/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/livestreammedia/visca27 /Users/livestreammedia/visca27 /Users/livestreammedia/visca27/build /Users/livestreammedia/visca27/build /Users/livestreammedia/visca27/build/CMakeFiles/visca27.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/visca27.dir/depend
