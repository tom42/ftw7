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
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <Windows.h>
#include "GLFW/glfw3.h"
#include "glfw/glfw3native.h"
#include "ftw7_version.h"
#include "ftw7_conemu/display/opengl_display_driver.hpp"
#include "ftw7_core/log/log.hpp"
#include "ftw7_core/windows/string.hpp"
#include "ftw7_core/wexcept.hpp"
#include "resource.h"

#define FTW7_OPENGL_DISPLAY_DRIVER_NAME PACKAGE_STRING " OpenGL display driver"

namespace ftw7_conemu
{
namespace display
{
namespace
{

void error_callback(int /*error*/, const char* description)
{
    FTW7_LOG_ERROR << L"glfw: " << description;
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

GLFWwindow* create_window(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    GLFWwindow* window = nullptr;
    try
    {
        if (settings.fullscreen)
        {
            auto monitor = glfwGetPrimaryMonitor();
            if (!monitor)
            {
                throw ftw7_core::wruntime_error(L"glfwGetPrimaryMonitor failed");
            }
            window = glfwCreateWindow(settings.screen_width, settings.screen_height, FTW7_OPENGL_DISPLAY_DRIVER_NAME, monitor, nullptr);
        }
        else
        {
            window = glfwCreateWindow(settings.window_width, settings.window_height, FTW7_OPENGL_DISPLAY_DRIVER_NAME, nullptr, nullptr);
        }        
        if (!window)
        {
            throw ftw7_core::wruntime_error(L"glfwCreateWindow failed");
        }

        // GLFW's mechanism of setting the icon doesn't work, since it assumes that
        // the Window has been created in an EXE rather than a DLL and uses GetModuleHandle(NULL)
        // as the first argument to LoadIconW.
        auto icon = LoadIconW(emulation_dll_module_handle, MAKEINTRESOURCEW(IDI_FTW7));
        SendMessageW(glfwGetWin32Window(window), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
        SendMessageW(glfwGetWin32Window(window), WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));

        glfwMakeContextCurrent(window);
        if (settings.fullscreen)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        glfwSetKeyCallback(window, key_callback);
        glfwSwapInterval(1);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window); 
        return window;
    }
    catch (...)
    {
        if (window)
        {
            glfwDestroyWindow(window);
        }
        throw;
    }
}

}

opengl_display_driver::opengl_display_driver(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
    : m_glfw_initialized(false),
    m_window(nullptr)
{
    try
    {
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
        {
            throw ftw7_core::wruntime_error(L"glfwInit failed");
        }
        m_glfw_initialized = true;
        m_window = create_window(emulation_dll_module_handle, settings);
    }
    catch (...)
    {
        close();
        throw;
    }
}

opengl_display_driver::~opengl_display_driver()
{
    close();
}

void opengl_display_driver::close()
{
    close_window();
    close_glfw();
}

void opengl_display_driver::close_window()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

void opengl_display_driver::close_glfw()
{
    if (m_glfw_initialized)
    {
        glfwTerminate();
        m_glfw_initialized = false;
    }
}

bool opengl_display_driver::handle_messages()
{
    glfwPollEvents();
    return !glfwWindowShouldClose(m_window);
}

void opengl_display_driver::render(const CHAR_INFO* /*buffer*/)
{
    // TODO: real rendering code
    float ratio;
    int width, height;

    glfwGetFramebufferSize(m_window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);
    glEnd();
    glfwSwapBuffers(m_window);
}

void opengl_display_driver::set_title(const wchar_t* title)
{
    // The GLFW documentation states that glfwSetWindowTitle's title argument is an UTF8 encoded string.
    const auto narrow_title = ftw7_core::windows::wstring_to_multibyte(CP_UTF8, title);
    glfwSetWindowTitle(m_window, narrow_title.c_str());
}

}
}
