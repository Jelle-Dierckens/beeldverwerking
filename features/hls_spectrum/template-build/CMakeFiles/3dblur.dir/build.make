# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_SOURCE_DIR = /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build

# Include any dependencies generated for this target.
include CMakeFiles/3dblur.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/3dblur.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/3dblur.dir/flags.make

CMakeFiles/3dblur.dir/src/3dblur.o: CMakeFiles/3dblur.dir/flags.make
CMakeFiles/3dblur.dir/src/3dblur.o: /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/src/3dblur.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/3dblur.dir/src/3dblur.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/3dblur.dir/src/3dblur.o -c /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/src/3dblur.cpp

CMakeFiles/3dblur.dir/src/3dblur.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/3dblur.dir/src/3dblur.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/src/3dblur.cpp > CMakeFiles/3dblur.dir/src/3dblur.i

CMakeFiles/3dblur.dir/src/3dblur.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/3dblur.dir/src/3dblur.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/src/3dblur.cpp -o CMakeFiles/3dblur.dir/src/3dblur.s

CMakeFiles/3dblur.dir/src/3dblur.o.requires:
.PHONY : CMakeFiles/3dblur.dir/src/3dblur.o.requires

CMakeFiles/3dblur.dir/src/3dblur.o.provides: CMakeFiles/3dblur.dir/src/3dblur.o.requires
	$(MAKE) -f CMakeFiles/3dblur.dir/build.make CMakeFiles/3dblur.dir/src/3dblur.o.provides.build
.PHONY : CMakeFiles/3dblur.dir/src/3dblur.o.provides

CMakeFiles/3dblur.dir/src/3dblur.o.provides.build: CMakeFiles/3dblur.dir/src/3dblur.o

# Object files for target 3dblur
3dblur_OBJECTS = \
"CMakeFiles/3dblur.dir/src/3dblur.o"

# External object files for target 3dblur
3dblur_EXTERNAL_OBJECTS =

/home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/bin/3dblur: CMakeFiles/3dblur.dir/src/3dblur.o
/home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/bin/3dblur: CMakeFiles/3dblur.dir/build.make
/home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/bin/3dblur: CMakeFiles/3dblur.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/bin/3dblur"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/3dblur.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/3dblur.dir/build: /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template/bin/3dblur
.PHONY : CMakeFiles/3dblur.dir/build

CMakeFiles/3dblur.dir/requires: CMakeFiles/3dblur.dir/src/3dblur.o.requires
.PHONY : CMakeFiles/3dblur.dir/requires

CMakeFiles/3dblur.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/3dblur.dir/cmake_clean.cmake
.PHONY : CMakeFiles/3dblur.dir/clean

CMakeFiles/3dblur.dir/depend:
	cd /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build /home/jelle/school/beeldverwerking/project/features/hls_spectrum/template-build/CMakeFiles/3dblur.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/3dblur.dir/depend
