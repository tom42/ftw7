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
project (mhook)
enable_testing ()

set (
  mhook_sources
  disasm-lib/cpu.c
  disasm-lib/cpu.h
  disasm-lib/disasm.c
  disasm-lib/disasm.h
  disasm-lib/disasm_x86.c
  disasm-lib/disasm_x86.h
  disasm-lib/disasm_x86_tables.h
  disasm-lib/misc.c
  disasm-lib/misc.h
  mhook-lib/mhook.cpp
  mhook-lib/mhook.h
)

set (
  mhook-test_sources
  mhook-test.cpp
  stdafx.cpp
  stdafx.h
)

if (MSVC)
  # Treat warnings as errors, but don't bother raising the warning level.
  # Mhook won't build without any warnings on /W4
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")

  # Disable warnings introduced after VS2013.
  if (MSVC_VERSION GREATER_EQUAL 1900)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18")
  endif ()
endif ()

add_library (mhook ${mhook_sources})

add_executable (mhook-test ${mhook-test_sources})
target_link_libraries (mhook-test mhook)
add_test (mhook-test mhook-test)
