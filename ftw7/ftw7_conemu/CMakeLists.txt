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
  ftw7_conemu_sources
  src/ftw7_conemu.def
  src/main.cpp
)

include_directories ("${PROJECT_SOURCE_DIR}/ftw7/ftw7_core/include")
add_library (ftw7_conemu SHARED ${ftw7_conemu_sources})
target_link_libraries (ftw7_conemu ftw7_core)
