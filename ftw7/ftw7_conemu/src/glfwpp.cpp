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
#include "GLFW/glfw3.h"
#include "ftw7_conemu/display/glfwpp.hpp"
#include "ftw7_core/wexcept.hpp"

namespace ftw7_conemu
{
namespace display
{
namespace glfw
{

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

monitor glfw::get_primary_monitor() const
{
    // TODO: better error handling: can we get at the last error fired by glfw and include that in the exception message???
    auto glfw_monitor = glfwGetPrimaryMonitor();
    if (!glfw_monitor)
    {
        throw ftw7_core::wruntime_error(L"glfwGetPrimaryMonitor failed");
    }
    return glfw_monitor;
}

}
}
}
