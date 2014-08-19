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

# Compile GLFW only.
set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set (GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set (GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set (GLFW_INSTALL OFF CACHE BOOL "Generate installation target")

# Include prologue header, so that GLFW can use real functions and not hooks.
add_definitions (-DFTW7_COMPILING_GLFW)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FI${PROJECT_BINARY_DIR}/hooked_functions.h")

add_subdirectory (glfw)

# Ensure generated ftw7 headers are built before GLFW.
add_dependencies (glfw ftw7_generated_headers)
