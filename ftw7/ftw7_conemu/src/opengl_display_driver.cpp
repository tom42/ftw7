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
#include <Windows.h>
#include "GLFW/glfw3.h"
#include "ftw7_conemu/display/opengl_display_driver.hpp"
#include "ftw7_core/log/log.hpp"

namespace ftw7_conemu
{
namespace display
{

opengl_display_driver::opengl_display_driver(HINSTANCE /*emulation_dll_module_handle*/, const ftw7_core::emulation::settings& /*settings*/)
{
    // TODO: initialize glfw, or die on error
}

opengl_display_driver::~opengl_display_driver()
{
    // TODO: clean up
}

bool opengl_display_driver::handle_messages()
{
    // TODO: handle glfw messages and stuff
    return true;
}

void opengl_display_driver::render(const CHAR_INFO* /*buffer*/)
{
    // TODO: render and swap buffers
}

void opengl_display_driver::set_title(const wchar_t* /*title*/)
{
    // TODO: set the title
}

}
}
