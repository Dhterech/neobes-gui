# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /home/zungle/src/neobesms

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zungle/src/neobesms

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/zungle/src/neobesms/CMakeFiles /home/zungle/src/neobesms//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/zungle/src/neobesms/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named neobesms

# Build rule for target.
neobesms: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 neobesms
.PHONY : neobesms

# fast build rule for target.
neobesms/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/build
.PHONY : neobesms/fast

#=============================================================================
# Target rules for targets named neobesms_autogen_timestamp_deps

# Build rule for target.
neobesms_autogen_timestamp_deps: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 neobesms_autogen_timestamp_deps
.PHONY : neobesms_autogen_timestamp_deps

# fast build rule for target.
neobesms_autogen_timestamp_deps/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms_autogen_timestamp_deps.dir/build.make CMakeFiles/neobesms_autogen_timestamp_deps.dir/build
.PHONY : neobesms_autogen_timestamp_deps/fast

#=============================================================================
# Target rules for targets named neobesms_autogen

# Build rule for target.
neobesms_autogen: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 neobesms_autogen
.PHONY : neobesms_autogen

# fast build rule for target.
neobesms_autogen/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms_autogen.dir/build.make CMakeFiles/neobesms_autogen.dir/build
.PHONY : neobesms_autogen/fast

adpcm.o: adpcm.cpp.o
.PHONY : adpcm.o

# target to build an object file
adpcm.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/adpcm.cpp.o
.PHONY : adpcm.cpp.o

adpcm.i: adpcm.cpp.i
.PHONY : adpcm.i

# target to preprocess a source file
adpcm.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/adpcm.cpp.i
.PHONY : adpcm.cpp.i

adpcm.s: adpcm.cpp.s
.PHONY : adpcm.s

# target to generate assembly for a file
adpcm.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/adpcm.cpp.s
.PHONY : adpcm.cpp.s

bdutil.o: bdutil.cpp.o
.PHONY : bdutil.o

# target to build an object file
bdutil.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/bdutil.cpp.o
.PHONY : bdutil.cpp.o

bdutil.i: bdutil.cpp.i
.PHONY : bdutil.i

# target to preprocess a source file
bdutil.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/bdutil.cpp.i
.PHONY : bdutil.cpp.i

bdutil.s: bdutil.cpp.s
.PHONY : bdutil.s

# target to generate assembly for a file
bdutil.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/bdutil.cpp.s
.PHONY : bdutil.cpp.s

intcommand.o: intcommand.cpp.o
.PHONY : intcommand.o

# target to build an object file
intcommand.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/intcommand.cpp.o
.PHONY : intcommand.cpp.o

intcommand.i: intcommand.cpp.i
.PHONY : intcommand.i

# target to preprocess a source file
intcommand.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/intcommand.cpp.i
.PHONY : intcommand.cpp.i

intcommand.s: intcommand.cpp.s
.PHONY : intcommand.s

# target to generate assembly for a file
intcommand.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/intcommand.cpp.s
.PHONY : intcommand.cpp.s

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/main.cpp.s
.PHONY : main.cpp.s

neobes.o: neobes.cpp.o
.PHONY : neobes.o

# target to build an object file
neobes.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobes.cpp.o
.PHONY : neobes.cpp.o

neobes.i: neobes.cpp.i
.PHONY : neobes.i

# target to preprocess a source file
neobes.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobes.cpp.i
.PHONY : neobes.cpp.i

neobes.s: neobes.cpp.s
.PHONY : neobes.s

# target to generate assembly for a file
neobes.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobes.cpp.s
.PHONY : neobes.cpp.s

neobesms_autogen/EWIEGA46WW/qrc_resources.o: neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.o
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.o

# target to build an object file
neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.o
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.o

neobesms_autogen/EWIEGA46WW/qrc_resources.i: neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.i
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.i

# target to preprocess a source file
neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.i
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.i

neobesms_autogen/EWIEGA46WW/qrc_resources.s: neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.s
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.s

# target to generate assembly for a file
neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.s
.PHONY : neobesms_autogen/EWIEGA46WW/qrc_resources.cpp.s

neobesms_autogen/mocs_compilation.o: neobesms_autogen/mocs_compilation.cpp.o
.PHONY : neobesms_autogen/mocs_compilation.o

# target to build an object file
neobesms_autogen/mocs_compilation.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/mocs_compilation.cpp.o
.PHONY : neobesms_autogen/mocs_compilation.cpp.o

neobesms_autogen/mocs_compilation.i: neobesms_autogen/mocs_compilation.cpp.i
.PHONY : neobesms_autogen/mocs_compilation.i

# target to preprocess a source file
neobesms_autogen/mocs_compilation.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/mocs_compilation.cpp.i
.PHONY : neobesms_autogen/mocs_compilation.cpp.i

neobesms_autogen/mocs_compilation.s: neobesms_autogen/mocs_compilation.cpp.s
.PHONY : neobesms_autogen/mocs_compilation.s

# target to generate assembly for a file
neobesms_autogen/mocs_compilation.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neobesms_autogen/mocs_compilation.cpp.s
.PHONY : neobesms_autogen/mocs_compilation.cpp.s

neodata.o: neodata.cpp.o
.PHONY : neodata.o

# target to build an object file
neodata.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neodata.cpp.o
.PHONY : neodata.cpp.o

neodata.i: neodata.cpp.i
.PHONY : neodata.i

# target to preprocess a source file
neodata.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neodata.cpp.i
.PHONY : neodata.cpp.i

neodata.s: neodata.cpp.s
.PHONY : neodata.s

# target to generate assembly for a file
neodata.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neodata.cpp.s
.PHONY : neodata.cpp.s

neosound.o: neosound.cpp.o
.PHONY : neosound.o

# target to build an object file
neosound.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neosound.cpp.o
.PHONY : neosound.cpp.o

neosound.i: neosound.cpp.i
.PHONY : neosound.i

# target to preprocess a source file
neosound.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neosound.cpp.i
.PHONY : neosound.cpp.i

neosound.s: neosound.cpp.s
.PHONY : neosound.s

# target to generate assembly for a file
neosound.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/neosound.cpp.s
.PHONY : neosound.cpp.s

pcsx2reader.o: pcsx2reader.cpp.o
.PHONY : pcsx2reader.o

# target to build an object file
pcsx2reader.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2reader.cpp.o
.PHONY : pcsx2reader.cpp.o

pcsx2reader.i: pcsx2reader.cpp.i
.PHONY : pcsx2reader.i

# target to preprocess a source file
pcsx2reader.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2reader.cpp.i
.PHONY : pcsx2reader.cpp.i

pcsx2reader.s: pcsx2reader.cpp.s
.PHONY : pcsx2reader.s

# target to generate assembly for a file
pcsx2reader.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2reader.cpp.s
.PHONY : pcsx2reader.cpp.s

pcsx2upload.o: pcsx2upload.cpp.o
.PHONY : pcsx2upload.o

# target to build an object file
pcsx2upload.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2upload.cpp.o
.PHONY : pcsx2upload.cpp.o

pcsx2upload.i: pcsx2upload.cpp.i
.PHONY : pcsx2upload.i

# target to preprocess a source file
pcsx2upload.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2upload.cpp.i
.PHONY : pcsx2upload.cpp.i

pcsx2upload.s: pcsx2upload.cpp.s
.PHONY : pcsx2upload.s

# target to generate assembly for a file
pcsx2upload.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2upload.cpp.s
.PHONY : pcsx2upload.cpp.s

pcsx2util.o: pcsx2util.cpp.o
.PHONY : pcsx2util.o

# target to build an object file
pcsx2util.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2util.cpp.o
.PHONY : pcsx2util.cpp.o

pcsx2util.i: pcsx2util.cpp.i
.PHONY : pcsx2util.i

# target to preprocess a source file
pcsx2util.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2util.cpp.i
.PHONY : pcsx2util.cpp.i

pcsx2util.s: pcsx2util.cpp.s
.PHONY : pcsx2util.s

# target to generate assembly for a file
pcsx2util.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/pcsx2util.cpp.s
.PHONY : pcsx2util.cpp.s

stageinfo.o: stageinfo.cpp.o
.PHONY : stageinfo.o

# target to build an object file
stageinfo.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/stageinfo.cpp.o
.PHONY : stageinfo.cpp.o

stageinfo.i: stageinfo.cpp.i
.PHONY : stageinfo.i

# target to preprocess a source file
stageinfo.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/stageinfo.cpp.i
.PHONY : stageinfo.cpp.i

stageinfo.s: stageinfo.cpp.s
.PHONY : stageinfo.s

# target to generate assembly for a file
stageinfo.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/stageinfo.cpp.s
.PHONY : stageinfo.cpp.s

suggest.o: suggest.cpp.o
.PHONY : suggest.o

# target to build an object file
suggest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/suggest.cpp.o
.PHONY : suggest.cpp.o

suggest.i: suggest.cpp.i
.PHONY : suggest.i

# target to preprocess a source file
suggest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/suggest.cpp.i
.PHONY : suggest.cpp.i

suggest.s: suggest.cpp.s
.PHONY : suggest.s

# target to generate assembly for a file
suggest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/neobesms.dir/build.make CMakeFiles/neobesms.dir/suggest.cpp.s
.PHONY : suggest.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... neobesms_autogen"
	@echo "... neobesms_autogen_timestamp_deps"
	@echo "... neobesms"
	@echo "... adpcm.o"
	@echo "... adpcm.i"
	@echo "... adpcm.s"
	@echo "... bdutil.o"
	@echo "... bdutil.i"
	@echo "... bdutil.s"
	@echo "... intcommand.o"
	@echo "... intcommand.i"
	@echo "... intcommand.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... neobes.o"
	@echo "... neobes.i"
	@echo "... neobes.s"
	@echo "... neobesms_autogen/EWIEGA46WW/qrc_resources.o"
	@echo "... neobesms_autogen/EWIEGA46WW/qrc_resources.i"
	@echo "... neobesms_autogen/EWIEGA46WW/qrc_resources.s"
	@echo "... neobesms_autogen/mocs_compilation.o"
	@echo "... neobesms_autogen/mocs_compilation.i"
	@echo "... neobesms_autogen/mocs_compilation.s"
	@echo "... neodata.o"
	@echo "... neodata.i"
	@echo "... neodata.s"
	@echo "... neosound.o"
	@echo "... neosound.i"
	@echo "... neosound.s"
	@echo "... pcsx2reader.o"
	@echo "... pcsx2reader.i"
	@echo "... pcsx2reader.s"
	@echo "... pcsx2upload.o"
	@echo "... pcsx2upload.i"
	@echo "... pcsx2upload.s"
	@echo "... pcsx2util.o"
	@echo "... pcsx2util.i"
	@echo "... pcsx2util.s"
	@echo "... stageinfo.o"
	@echo "... stageinfo.i"
	@echo "... stageinfo.s"
	@echo "... suggest.o"
	@echo "... suggest.i"
	@echo "... suggest.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

