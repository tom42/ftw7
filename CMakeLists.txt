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

cmake_minimum_required (VERSION 3.7.0)
project (ftw7)
enable_testing ()

set (FTW7_GENERATED_SOURCES_DIR "${PROJECT_BINARY_DIR}")

add_definitions (-DUNICODE)
add_definitions (-D_UNICODE)

add_subdirectory (argp)
add_subdirectory (glfw)
add_subdirectory (mhook)
add_subdirectory (ftw7)
