# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\user software\utilities\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\user software\utilities\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\user files\file_system\fs"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\user files\file_system\fs\cmake-build-debug-mingw"

# Include any dependencies generated for this target.
include CMakeFiles/fs.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/fs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fs.dir/flags.make

CMakeFiles/fs.dir/main.c.obj: CMakeFiles/fs.dir/flags.make
CMakeFiles/fs.dir/main.c.obj: CMakeFiles/fs.dir/includes_C.rsp
CMakeFiles/fs.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\user files\file_system\fs\cmake-build-debug-mingw\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/fs.dir/main.c.obj"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\fs.dir\main.c.obj -c "C:\user files\file_system\fs\main.c"

CMakeFiles/fs.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fs.dir/main.c.i"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\user files\file_system\fs\main.c" > CMakeFiles\fs.dir\main.c.i

CMakeFiles/fs.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fs.dir/main.c.s"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\user files\file_system\fs\main.c" -o CMakeFiles\fs.dir\main.c.s

CMakeFiles/fs.dir/src/myfs.c.obj: CMakeFiles/fs.dir/flags.make
CMakeFiles/fs.dir/src/myfs.c.obj: CMakeFiles/fs.dir/includes_C.rsp
CMakeFiles/fs.dir/src/myfs.c.obj: ../src/myfs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\user files\file_system\fs\cmake-build-debug-mingw\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/fs.dir/src/myfs.c.obj"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\fs.dir\src\myfs.c.obj -c "C:\user files\file_system\fs\src\myfs.c"

CMakeFiles/fs.dir/src/myfs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fs.dir/src/myfs.c.i"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\user files\file_system\fs\src\myfs.c" > CMakeFiles\fs.dir\src\myfs.c.i

CMakeFiles/fs.dir/src/myfs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fs.dir/src/myfs.c.s"
	C:\USERSO~1\UTILIT~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\user files\file_system\fs\src\myfs.c" -o CMakeFiles\fs.dir\src\myfs.c.s

# Object files for target fs
fs_OBJECTS = \
"CMakeFiles/fs.dir/main.c.obj" \
"CMakeFiles/fs.dir/src/myfs.c.obj"

# External object files for target fs
fs_EXTERNAL_OBJECTS =

fs.exe: CMakeFiles/fs.dir/main.c.obj
fs.exe: CMakeFiles/fs.dir/src/myfs.c.obj
fs.exe: CMakeFiles/fs.dir/build.make
fs.exe: CMakeFiles/fs.dir/linklibs.rsp
fs.exe: CMakeFiles/fs.dir/objects1.rsp
fs.exe: CMakeFiles/fs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\user files\file_system\fs\cmake-build-debug-mingw\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable fs.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\fs.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fs.dir/build: fs.exe
.PHONY : CMakeFiles/fs.dir/build

CMakeFiles/fs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\fs.dir\cmake_clean.cmake
.PHONY : CMakeFiles/fs.dir/clean

CMakeFiles/fs.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\user files\file_system\fs" "C:\user files\file_system\fs" "C:\user files\file_system\fs\cmake-build-debug-mingw" "C:\user files\file_system\fs\cmake-build-debug-mingw" "C:\user files\file_system\fs\cmake-build-debug-mingw\CMakeFiles\fs.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/fs.dir/depend

