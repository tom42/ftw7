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

set (hooked_functions_h "${FTW7_GENERATED_SOURCES_DIR}/hooked_functions.h")

set (
  ftw7_conemu_sources
  src/emulation.cpp
  src/ftw7_conemu.def
  src/ftw7_conemu.rc
  src/gdi_display_driver.cpp
  src/main.cpp
  src/opengl_display_driver.cpp
  src/true_functions.c
  src/vga8x8.cpp
)

add_custom_command (
  OUTPUT "${hooked_functions_h}"
  COMMAND codegenerator > "${hooked_functions_h}"
  DEPENDS codegenerator
)

add_custom_target (
  ftw7_generated_sources
  SOURCES
  "${hooked_functions_h}"
)

include_directories ("${PROJECT_SOURCE_DIR}/ftw7/ftw7_conemu/include")
include_directories ("${PROJECT_SOURCE_DIR}/ftw7/ftw7_core/include")
include_directories ("${PROJECT_SOURCE_DIR}/mhook")
include_directories ("${PROJECT_SOURCE_DIR}/glfw/glfw/include")
add_library (ftw7_conemu SHARED ${ftw7_conemu_sources})
add_dependencies (ftw7_conemu ftw7_generated_sources)
target_link_libraries (ftw7_conemu ftw7_core glfw mhook opengl32)
