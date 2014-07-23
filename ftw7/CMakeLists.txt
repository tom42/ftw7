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

################################################################################
# Output directories.
# Put all ftw7 specific output files into a common directory to simplify
# running and debugging ftw7.
################################################################################

set (FTW7_COMMON_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set (
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    "${FTW7_COMMON_OUTPUT_DIRECTORY}"
    CACHE PATH
    "Single directory for all libraries."
)

set (
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${FTW7_COMMON_OUTPUT_DIRECTORY}"
    CACHE PATH
    "Single directory for all executables."
)

set (
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    "${FTW7_COMMON_OUTPUT_DIRECTORY}"
    CACHE PATH
    "Single directory for all static libraries."
)
