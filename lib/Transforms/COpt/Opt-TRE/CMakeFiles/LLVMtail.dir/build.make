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
CMAKE_SOURCE_DIR = /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE

# Include any dependencies generated for this target.
include CMakeFiles/LLVMtail.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/LLVMtail.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LLVMtail.dir/flags.make

CMakeFiles/LLVMtail.dir/source/Optimiser.o: CMakeFiles/LLVMtail.dir/flags.make
CMakeFiles/LLVMtail.dir/source/Optimiser.o: source/Optimiser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LLVMtail.dir/source/Optimiser.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LLVMtail.dir/source/Optimiser.o -c /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/source/Optimiser.cpp

CMakeFiles/LLVMtail.dir/source/Optimiser.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMtail.dir/source/Optimiser.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/source/Optimiser.cpp > CMakeFiles/LLVMtail.dir/source/Optimiser.i

CMakeFiles/LLVMtail.dir/source/Optimiser.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMtail.dir/source/Optimiser.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/source/Optimiser.cpp -o CMakeFiles/LLVMtail.dir/source/Optimiser.s

CMakeFiles/LLVMtail.dir/source/Optimiser.o.requires:

.PHONY : CMakeFiles/LLVMtail.dir/source/Optimiser.o.requires

CMakeFiles/LLVMtail.dir/source/Optimiser.o.provides: CMakeFiles/LLVMtail.dir/source/Optimiser.o.requires
	$(MAKE) -f CMakeFiles/LLVMtail.dir/build.make CMakeFiles/LLVMtail.dir/source/Optimiser.o.provides.build
.PHONY : CMakeFiles/LLVMtail.dir/source/Optimiser.o.provides

CMakeFiles/LLVMtail.dir/source/Optimiser.o.provides.build: CMakeFiles/LLVMtail.dir/source/Optimiser.o


# Object files for target LLVMtail
LLVMtail_OBJECTS = \
"CMakeFiles/LLVMtail.dir/source/Optimiser.o"

# External object files for target LLVMtail
LLVMtail_EXTERNAL_OBJECTS =

lib/LLVMtail.so: CMakeFiles/LLVMtail.dir/source/Optimiser.o
lib/LLVMtail.so: CMakeFiles/LLVMtail.dir/build.make
lib/LLVMtail.so: CMakeFiles/LLVMtail.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module lib/LLVMtail.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMtail.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LLVMtail.dir/build: lib/LLVMtail.so

.PHONY : CMakeFiles/LLVMtail.dir/build

CMakeFiles/LLVMtail.dir/requires: CMakeFiles/LLVMtail.dir/source/Optimiser.o.requires

.PHONY : CMakeFiles/LLVMtail.dir/requires

CMakeFiles/LLVMtail.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LLVMtail.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LLVMtail.dir/clean

CMakeFiles/LLVMtail.dir/depend:
	cd /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE /home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-TRE/CMakeFiles/LLVMtail.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LLVMtail.dir/depend

