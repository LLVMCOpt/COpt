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
CMAKE_SOURCE_DIR = "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding"

# Include any dependencies generated for this target.
include CMakeFiles/LLVMconstfold.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/LLVMconstfold.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LLVMconstfold.dir/flags.make

CMakeFiles/LLVMconstfold.dir/source/Optimiser.o: CMakeFiles/LLVMconstfold.dir/flags.make
CMakeFiles/LLVMconstfold.dir/source/Optimiser.o: source/Optimiser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LLVMconstfold.dir/source/Optimiser.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LLVMconstfold.dir/source/Optimiser.o -c "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/source/Optimiser.cpp"

CMakeFiles/LLVMconstfold.dir/source/Optimiser.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMconstfold.dir/source/Optimiser.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/source/Optimiser.cpp" > CMakeFiles/LLVMconstfold.dir/source/Optimiser.i

CMakeFiles/LLVMconstfold.dir/source/Optimiser.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMconstfold.dir/source/Optimiser.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/source/Optimiser.cpp" -o CMakeFiles/LLVMconstfold.dir/source/Optimiser.s

CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.requires:

.PHONY : CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.requires

CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.provides: CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.requires
	$(MAKE) -f CMakeFiles/LLVMconstfold.dir/build.make CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.provides.build
.PHONY : CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.provides

CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.provides.build: CMakeFiles/LLVMconstfold.dir/source/Optimiser.o


# Object files for target LLVMconstfold
LLVMconstfold_OBJECTS = \
"CMakeFiles/LLVMconstfold.dir/source/Optimiser.o"

# External object files for target LLVMconstfold
LLVMconstfold_EXTERNAL_OBJECTS =

lib/LLVMconstfold.so: CMakeFiles/LLVMconstfold.dir/source/Optimiser.o
lib/LLVMconstfold.so: CMakeFiles/LLVMconstfold.dir/build.make
lib/LLVMconstfold.so: CMakeFiles/LLVMconstfold.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module lib/LLVMconstfold.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMconstfold.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LLVMconstfold.dir/build: lib/LLVMconstfold.so

.PHONY : CMakeFiles/LLVMconstfold.dir/build

CMakeFiles/LLVMconstfold.dir/requires: CMakeFiles/LLVMconstfold.dir/source/Optimiser.o.requires

.PHONY : CMakeFiles/LLVMconstfold.dir/requires

CMakeFiles/LLVMconstfold.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LLVMconstfold.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LLVMconstfold.dir/clean

CMakeFiles/LLVMconstfold.dir/depend:
	cd "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding" "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding" "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding" "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding" "/home/sruthi/LLVM/installed/llvm-4.0.0.src/lib/Transforms/CompOptDSL/Opt-ConstantPropagation&Folding/CMakeFiles/LLVMconstfold.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/LLVMconstfold.dir/depend
