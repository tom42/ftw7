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

# Put all ftw7 specific binaries below this directory into a shared
# output directory. This simplifies running and debugging ftw7.
set (FTW7_SHARED_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FTW7_SHARED_OUTPUT_DIRECTORY}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FTW7_SHARED_OUTPUT_DIRECTORY}")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FTW7_SHARED_OUTPUT_DIRECTORY}")

if (MSVC)
  # Set warning level to 4 (/W4)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()

  # Treat warnings as errors
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
endif ()

add_subdirectory (ftw7)
