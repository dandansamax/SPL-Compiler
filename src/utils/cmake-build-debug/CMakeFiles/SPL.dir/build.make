# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /mnt/d/Projects/CS323Compiler/SPL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/SPL.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SPL.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SPL.dir/flags.make

CMakeFiles/SPL.dir/main.c.o: CMakeFiles/SPL.dir/flags.make
CMakeFiles/SPL.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SPL.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SPL.dir/main.c.o   -c /mnt/d/Projects/CS323Compiler/SPL/main.c

CMakeFiles/SPL.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SPL.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Projects/CS323Compiler/SPL/main.c > CMakeFiles/SPL.dir/main.c.i

CMakeFiles/SPL.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SPL.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Projects/CS323Compiler/SPL/main.c -o CMakeFiles/SPL.dir/main.c.s

CMakeFiles/SPL.dir/hash_table.c.o: CMakeFiles/SPL.dir/flags.make
CMakeFiles/SPL.dir/hash_table.c.o: ../hash_table.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/SPL.dir/hash_table.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SPL.dir/hash_table.c.o   -c /mnt/d/Projects/CS323Compiler/SPL/hash_table.c

CMakeFiles/SPL.dir/hash_table.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SPL.dir/hash_table.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Projects/CS323Compiler/SPL/hash_table.c > CMakeFiles/SPL.dir/hash_table.c.i

CMakeFiles/SPL.dir/hash_table.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SPL.dir/hash_table.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Projects/CS323Compiler/SPL/hash_table.c -o CMakeFiles/SPL.dir/hash_table.c.s

CMakeFiles/SPL.dir/symbol_table.c.o: CMakeFiles/SPL.dir/flags.make
CMakeFiles/SPL.dir/symbol_table.c.o: ../symbol_table.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/SPL.dir/symbol_table.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SPL.dir/symbol_table.c.o   -c /mnt/d/Projects/CS323Compiler/SPL/symbol_table.c

CMakeFiles/SPL.dir/symbol_table.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SPL.dir/symbol_table.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Projects/CS323Compiler/SPL/symbol_table.c > CMakeFiles/SPL.dir/symbol_table.c.i

CMakeFiles/SPL.dir/symbol_table.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SPL.dir/symbol_table.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Projects/CS323Compiler/SPL/symbol_table.c -o CMakeFiles/SPL.dir/symbol_table.c.s

CMakeFiles/SPL.dir/type.c.o: CMakeFiles/SPL.dir/flags.make
CMakeFiles/SPL.dir/type.c.o: ../type.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/SPL.dir/type.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SPL.dir/type.c.o   -c /mnt/d/Projects/CS323Compiler/SPL/type.c

CMakeFiles/SPL.dir/type.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SPL.dir/type.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Projects/CS323Compiler/SPL/type.c > CMakeFiles/SPL.dir/type.c.i

CMakeFiles/SPL.dir/type.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SPL.dir/type.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Projects/CS323Compiler/SPL/type.c -o CMakeFiles/SPL.dir/type.c.s

# Object files for target SPL
SPL_OBJECTS = \
"CMakeFiles/SPL.dir/main.c.o" \
"CMakeFiles/SPL.dir/hash_table.c.o" \
"CMakeFiles/SPL.dir/symbol_table.c.o" \
"CMakeFiles/SPL.dir/type.c.o"

# External object files for target SPL
SPL_EXTERNAL_OBJECTS =

SPL: CMakeFiles/SPL.dir/main.c.o
SPL: CMakeFiles/SPL.dir/hash_table.c.o
SPL: CMakeFiles/SPL.dir/symbol_table.c.o
SPL: CMakeFiles/SPL.dir/type.c.o
SPL: CMakeFiles/SPL.dir/build.make
SPL: CMakeFiles/SPL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable SPL"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SPL.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SPL.dir/build: SPL

.PHONY : CMakeFiles/SPL.dir/build

CMakeFiles/SPL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SPL.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SPL.dir/clean

CMakeFiles/SPL.dir/depend:
	cd /mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/Projects/CS323Compiler/SPL /mnt/d/Projects/CS323Compiler/SPL /mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug /mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug /mnt/d/Projects/CS323Compiler/SPL/cmake-build-debug/CMakeFiles/SPL.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SPL.dir/depend

