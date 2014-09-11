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
#ifndef FTW7_CONEMU_DISPLAY_GLFWPP_HPP_INCLUDED
#define FTW7_CONEMU_DISPLAY_GLFWPP_HPP_INCLUDED

#include <Windows.h>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

struct GLFWmonitor;
struct GLFWwindow;

namespace ftw7_conemu
{
namespace display
{
namespace glfw
{

class monitor
{
public:
    explicit monitor(GLFWmonitor* monitor);
    const wchar_t* display_name() const;
    GLFWmonitor* get() const { return m_monitor; }
private:
    // GLFWmonitors are owned by the GLFW library, not the monitor class.
    GLFWmonitor* m_monitor;
};

class window
{
public:
    typedef std::function<void(window& window, int key, int scancode, int action, int mods)> key_callback_t;

    explicit window(GLFWwindow* window = nullptr) throw();
    window(window&& other) throw();
    window& operator=(window&& other) throw();

    std::pair<int, int> framebuffer_size();
    void input_mode(int mode, int value);
    void key_callback(const key_callback_t& callback);
    void make_context_current();
    bool should_close();
    void swap_buffers();
    void title(const char* title);
    HWND win32_window();
private:
    window(const window&) = delete;
    window& operator = (const window&) = delete;

    GLFWwindow* get_glfw_window();
    void set_glfw_window(GLFWwindow* window) throw();

    static void key_callback_stub(GLFWwindow* w, int key, int scancode, int action, int mods);

    struct GLFWwindow_deleter
    {
        void operator()(GLFWwindow* window);
    };

    std::unique_ptr<GLFWwindow, GLFWwindow_deleter> m_window;
    key_callback_t m_key_callback;
};

class glfw
{
public:
    glfw();
    ~glfw();

    std::vector<monitor> get_monitors();
    monitor get_primary_monitor();

    window create_window(int width, int height, const char* title, const monitor* monitor);
    void window_hint(int target, int hint);

    void poll_events();
    void swap_interval(int interval);

private:
    glfw(const glfw&) = delete;
    glfw& operator = (const glfw&) = delete;
};

}
}
}

#endif
