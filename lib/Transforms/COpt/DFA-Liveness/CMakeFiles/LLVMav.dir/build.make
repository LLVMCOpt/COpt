# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness

# Include any dependencies generated for this target.
include CMakeFiles/LLVMav.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/LLVMav.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LLVMav.dir/flags.make

CMakeFiles/LLVMav.dir/sourcecode/liveness.o: CMakeFiles/LLVMav.dir/flags.make
CMakeFiles/LLVMav.dir/sourcecode/liveness.o: sourcecode/liveness.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LLVMav.dir/sourcecode/liveness.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LLVMav.dir/sourcecode/liveness.o -c /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/sourcecode/liveness.cpp

CMakeFiles/LLVMav.dir/sourcecode/liveness.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMav.dir/sourcecode/liveness.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/sourcecode/liveness.cpp > CMakeFiles/LLVMav.dir/sourcecode/liveness.i

CMakeFiles/LLVMav.dir/sourcecode/liveness.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMav.dir/sourcecode/liveness.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/sourcecode/liveness.cpp -o CMakeFiles/LLVMav.dir/sourcecode/liveness.s

CMakeFiles/LLVMav.dir/sourcecode/liveness.o.requires:

.PHONY : CMakeFiles/LLVMav.dir/sourcecode/liveness.o.requires

CMakeFiles/LLVMav.dir/sourcecode/liveness.o.provides: CMakeFiles/LLVMav.dir/sourcecode/liveness.o.requires
	$(MAKE) -f CMakeFiles/LLVMav.dir/build.make CMakeFiles/LLVMav.dir/sourcecode/liveness.o.provides.build
.PHONY : CMakeFiles/LLVMav.dir/sourcecode/liveness.o.provides

CMakeFiles/LLVMav.dir/sourcecode/liveness.o.provides.build: CMakeFiles/LLVMav.dir/sourcecode/liveness.o


# Object files for target LLVMav
LLVMav_OBJECTS = \
"CMakeFiles/LLVMav.dir/sourcecode/liveness.o"

# External object files for target LLVMav
LLVMav_EXTERNAL_OBJECTS =

lib/LLVMav.so: CMakeFiles/LLVMav.dir/sourcecode/liveness.o
lib/LLVMav.so: CMakeFiles/LLVMav.dir/build.make
lib/LLVMav.so: CMakeFiles/LLVMav.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module lib/LLVMav.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMav.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LLVMav.dir/build: lib/LLVMav.so

.PHONY : CMakeFiles/LLVMav.dir/build

CMakeFiles/LLVMav.dir/requires: CMakeFiles/LLVMav.dir/sourcecode/liveness.o.requires

.PHONY : CMakeFiles/LLVMav.dir/requires

CMakeFiles/LLVMav.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LLVMav.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LLVMav.dir/clean

CMakeFiles/LLVMav.dir/depend:
	cd /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Liveness/CMakeFiles/LLVMav.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LLVMav.dir/depend

