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

set (
  ftw7_core_sources
  src/demo.cpp
  src/format_message.cpp
  src/gpreg32.cpp
  src/string.cpp
)

set (
  ftw7_core_test_sources
  test/asm86_directives_test.cpp
  test/asm86_opcodes_test.cpp
  test/asm86_reference_test.cpp
  test/asm86_value_test.cpp
  test/format_message_test.cpp
  test/main.cpp
  test/object_test.cpp
)

include_directories ("${PROJECT_SOURCE_DIR}/ftw7/ftw7_core/include")
include_directories ("${Boost_INCLUDE_DIRS}")

# Currently we're using autolinking, which means we don't need to specify the
# Boost libraries using target_link_libraries. We must however ensure the
# directory containing the Boost libraries is in the library search path.
link_directories (${Boost_LIBRARY_DIRS})

add_library (ftw7_core ${ftw7_core_sources})

add_executable (ftw7_core_test ${ftw7_core_test_sources})
target_link_libraries (ftw7_core_test ftw7_core)
add_test (ftw7_core_test "${FTW7_SHARED_OUTPUT_DIRECTORY}/ftw7_core_test")
