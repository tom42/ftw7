/*
 * Copyright 2012-2014 Thomas Mathys
 *
 * This file is part of ftw7.
 *
 * ftw7 is free software : you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ftw7 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ftw7.If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstring>
#include "ftw7_version.h"
#include "ftw7_core/emulation/emulation.hpp"

namespace ftw7_core
{
namespace emulation
{

void settings::initialize(settings& settings)
{
    memset(&settings, 0, sizeof(settings));
    settings.size = sizeof(settings);

    // TODO: nasty, really.
    // * We should either at compile or runtime ensure that PACKAGE_STRING fits
    //   into magic_string. If not we fail compilation or execution. I can
    //   totally live with both. Just clamping PACKAGE_STRING by using strncpy
    //   is not nice, tho.
    strncpy(settings.magic_string, PACKAGE_STRING, sizeof(settings.magic_string));
}

}
}
