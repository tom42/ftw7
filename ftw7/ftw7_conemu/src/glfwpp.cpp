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
#include <stdexcept>
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

monitor::monitor(GLFWmonitor* monitor)
    : m_monitor(monitor)
{
    if (!monitor)
    {
        throw std::invalid_argument("monitor must not be null");
    }
}

const wchar_t* monitor::display_name() const
{
    return glfwGetWin32Monitor(m_monitor);
}


////////////////////////////////////////////////////////////////////////////////
// window
////////////////////////////////////////////////////////////////////////////////

window::window(GLFWwindow* window)
{
    set_glfw_window(window);
}

window::window(window&& other)
{
    set_glfw_window(other.m_window.release());
}

window& window::operator=(window&& other)
{
    set_glfw_window(other.m_window.release());
    return *this;
}

std::pair<int, int> window::framebuffer_size()
{
    int width, height;
    glfwGetFramebufferSize(get_glfw_window(), &width, &height);
    return std::make_pair(width, height);
}

void window::input_mode(int mode, int value)
{
    glfwSetInputMode(get_glfw_window(), mode, value);
}

void window::key_callback(const key_callback_t& /*callback*/)
{
    // TODO: tuck away callback
    // TODO: if callback is empty, remove it (from the glfwwindow and the window ipnstance)
    glfwSetKeyCallback(get_glfw_window(), key_callback_stub);
}

void window::make_context_current()
{
    glfwMakeContextCurrent(get_glfw_window());
}

bool window::should_close()
{
    return glfwWindowShouldClose(get_glfw_window()) != 0;
}

void window::swap_buffers()
{
    return glfwSwapBuffers(get_glfw_window());
}

void window::title(const char* title)
{
    glfwSetWindowTitle(get_glfw_window(), title);
}

HWND window::win32_window()
{
    return glfwGetWin32Window(get_glfw_window());
}

GLFWwindow* window::get_glfw_window()
{
    if (!m_window)
    {
        throw std::logic_error("m_window is null");
    }
    return m_window.get();
}

void window::set_glfw_window(GLFWwindow* window)
{
    m_window.reset(window);
    if (window)
    {
        glfwSetWindowUserPointer(window, this);
    }
}

void window::key_callback_stub(GLFWwindow* w, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
    // TODO: what do we do if user pointer is null?
    // TODO: what do we do if key callback is empty?
    auto window_instance = static_cast<window*>(glfwGetWindowUserPointer(w));
    if (window_instance) {} // TODO: actual call to real key callback....
}

void window::GLFWwindow_deleter::operator()(GLFWwindow* window)
{
    // glfwDestroyWindow allows closing of null pointers, so we don't need any checks here.
    glfwDestroyWindow(window);
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

std::vector<monitor> glfw::get_monitors()
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

monitor glfw::get_primary_monitor()
{
    // TODO: better error handling: can we get at the last error fired by glfw and include that in the exception message?
    auto glfw_monitor = glfwGetPrimaryMonitor();
    if (!glfw_monitor)
    {
        throw ftw7_core::wruntime_error(L"glfwGetPrimaryMonitor failed");
    }
    return monitor(glfw_monitor);
}

window glfw::create_window(int width, int height, const char* title, const monitor* monitor)
{
    // TODO: better error handling: can we get at the last error fired by glfw and include that in the exception message?
    auto glfw_monitor = monitor ? monitor->get() : nullptr;
    auto glfw_window = glfwCreateWindow(width, height, title, glfw_monitor, nullptr);
    if (!glfw_window)
    {
        throw ftw7_core::wruntime_error(L"glfwCreateWindow failed");
    }
    return window(glfw_window);
}

void glfw::window_hint(int target, int hint)
{
    glfwWindowHint(target, hint);
}

void glfw::poll_events()
{
    glfwPollEvents();
}

void glfw::swap_interval(int interval)
{
    glfwSwapInterval(interval);
}

}
}
}
