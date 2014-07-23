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

# strerror_r and strerror.
# We only check for functions and their declarations here.
# The decision which one to use is made in the argp sources.
check_function_exists (strerror_r HAVE_STRERROR_R)
check_symbol_exists (strerror_r string.h HAVE_DECL_STRERROR_R)
check_symbol_exists (strerror string.h HAVE_DECL_STRERROR)

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
  src/argp-ba.c
  src/argp-ba.c
  src/argp-eexst.c
  src/argp-fmtstream.c
  src/argp-fs-xinl.c
  src/argp-help.c
  src/argp-parse.c
  src/argp-pv.c
  src/argp-pvh.c
  src/argp-xinl.c
)

include_directories ("${PROJECT_SOURCE_DIR}/include/argp")
add_library (argp ${argp_sources})
