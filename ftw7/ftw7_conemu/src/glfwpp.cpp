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

// Include Windows.h before GLFW/glfw3native.h to avoid warnings about
// redefinitions of the APIENTRY macro.
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "ftw7_conemu/display/glfwpp.hpp"
#include "ftw7_core/wexcept.hpp"

namespace ftw7_conemu
{
namespace display
{
namespace glfw
{

////////////////////////////////////////////////////////////////////////////////
// monitor
////////////////////////////////////////////////////////////////////////////////

const wchar_t* monitor::display_name()
{
    return glfwGetWin32Monitor(m_monitor);
}


////////////////////////////////////////////////////////////////////////////////
// glfw
////////////////////////////////////////////////////////////////////////////////

glfw::glfw()
{
    // TODO: initialize glfw and install the error handler thingy. Throw on error.
    // TODO: singletonize this one way or another?
    // TODO: set up error callback, somehow
    //       Note: error messages are UTF8, so we have to convert them before sticking them into error messages.
    //       Also, note that we need to store them ourselves!
    if (!glfwInit())
    {
        throw ftw7_core::wruntime_error(L"glfwInit failed");
    }
}

glfw::~glfw()
{
    glfwTerminate();
}

std::vector<monitor> glfw::get_monitors() const
{
    int n_monitors;
    auto glfw_monitors = glfwGetMonitors(&n_monitors);
    if (!glfw_monitors)
    {
        // TODO: better error handling: can we get at the last error fired by glfw and include that in the exception message?
        throw ftw7_core::wruntime_error(L"glfwGetMonitors failed");
    }

    std::vector<monitor> monitors;
    monitors.reserve(n_monitors);
    for (int i = 0; i < n_monitors; ++i)
    {
        monitors.push_back(monitor(glfw_monitors[i]));
    }
    return monitors;
}

monitor glfw::get_primary_monitor() const
{
    // TODO: better error handling: can we get at the last error fired by glfw and include that in the exception message?
    auto glfw_monitor = glfwGetPrimaryMonitor();
    if (!glfw_monitor)
    {
        throw ftw7_core::wruntime_error(L"glfwGetPrimaryMonitor failed");
    }
    return monitor(glfw_monitor);
}

}
}
}
