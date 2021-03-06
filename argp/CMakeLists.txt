# Copyright 2012-2014 Thomas Mathys
#
# This file is part of ftw7.
#
# ftw7 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ftw7 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ftw7.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 3.0.0)
project (argp)
enable_testing ()

include (CheckFunctionExists)
include (CheckIncludeFile)
include (CheckSymbolExists)

################################################################################
# config.h
################################################################################

# Headers
check_include_file (mempcpy.h HAVE_MEMPCPY_H)
check_include_file (strcase.h HAVE_STRCASE_H)
check_include_file (strchrnul.h HAVE_STRCHRNUL_H)
check_include_file (strndup.h HAVE_STRNDUP_H)
check_include_file (sysexits.h HAVE_SYSEXITS_H)
check_include_file (unistd.h HAVE_UNISTD_H)

# Check for unlocked variants of stdio functions.
check_symbol_exists (putc_unlocked stdio.h HAVE_DECL_PUTC_UNLOCKED)
check_symbol_exists (fputs_unlocked stdio.h HAVE_DECL_FPUTS_UNLOCKED)
check_symbol_exists (fwrite_unlocked stdio.h HAVE_DECL_FWRITE_UNLOCKED)

# strerror_r and strerror.
# We only check for functions and their declarations here.
# The decision which one to use is made in the argp sources.
check_function_exists (strerror_r HAVE_STRERROR_R)
check_symbol_exists (strerror_r string.h HAVE_DECL_STRERROR_R)
check_symbol_exists (strerror string.h HAVE_DECL_STRERROR)

# Miscellaneous functions
check_function_exists (asprintf HAVE_ASPRINTF)
check_function_exists (mempcpy HAVE_MEMPCPY)
check_function_exists (random HAVE_RANDOM)
check_function_exists (sleep HAVE_SLEEP)
check_function_exists (strcasecmp HAVE_STRCASECMP)
check_function_exists (strchrnul HAVE_STRCHRNUL)
check_function_exists (strndup HAVE_STRNDUP)

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)
include_directories ("${CMAKE_CURRENT_BINARY_DIR}")
add_definitions (-DHAVE_CONFIG_H)

################################################################################
# Standalone argp library
################################################################################

set (
  argp_sources
  include/argp/argp.h
  src/argp-ba.c
  src/argp-compat.c
  src/argp-compat.h
  src/argp-eexst.c
  src/argp-fmtstream.c
  src/argp-fmtstream.h
  src/argp-fs-xinl.c
  src/argp-getopt.h
  src/argp-getopt_int.h
  src/argp-help.c
  src/argp-namefrob.h
  src/argp-parse.c
  src/argp-pv.c
  src/argp-pvh.c
  src/argp-xinl.c
  src/getopt.c
  src/getopt1.c
)

# Adds source files to source groups. Source group names are based on files'
# directory names, so that e.g. in Visual Studio we get a hierarchy of file
# filters that reflect the source tree's structure.
function (ftw7_source_group)
  foreach (source IN ITEMS ${ARGN})
    if (IS_ABSOLUTE)
      message (FATAL_ERROR "ftw7_source_group: absolute paths are not supported: ${source}")
    endif()
    get_filename_component (source_path "${source}" DIRECTORY)

    # Visual Studio file filters don't understand Unix path separators.
    # Convert to native path first.
    file (TO_NATIVE_PATH "${source_path}" native_path)
    source_group ("${native_path}" FILES "${source}")
  endforeach ()
endfunction ()

ftw7_source_group (${argp_sources})

if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)

  # VS2015: By default, __argc and __argv are macros, which interfere with
  # function declarations/definitions in argp. Disable this.
  add_definitions (-D_CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY)

  # Treat warnings as errors, but don't bother raising the warning level.
  # argp won't build without any warnings on /W4
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
endif ()
include_directories ("${PROJECT_SOURCE_DIR}/include/argp")
add_library (argp ${argp_sources})

################################################################################
# Tests
################################################################################

set (
  argp_test_sources
  src/argp-test.c
)

ftw7_source_group (${argp_test_sources})

if (MSVC)
  add_definitions (-D_CRT_NONSTDC_NO_WARNINGS)
endif ()
add_executable (argp-test ${argp_test_sources})
target_link_libraries (argp-test argp)
add_test (argp-test argp-test)
