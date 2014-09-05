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

#include <memory>
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
    explicit monitor(GLFWmonitor* monitor) : m_monitor(monitor) {}
    const wchar_t* display_name();
    GLFWmonitor* get() { return m_monitor; }
private:
    // GLFWmonitors are owned by the GLFW library, not the monitor class.
    GLFWmonitor* m_monitor;
};

class window
{
public:
    explicit window(GLFWwindow* window);
    bool should_close() const;
private:
    // Having a class member of type std::unique_ptr automatically makes us non-copyable.
    // Moreover we can let the compiler generate a move constructor and a move assignment operator.
    // See http://flamingdangerzone.com/cxx11/2012/08/15/rule-of-zero.html.
    struct GLFWwindow_deleter
    {
        void operator()(GLFWwindow* window);
    };
    std::unique_ptr<GLFWwindow, GLFWwindow_deleter> m_window;
};

class glfw
{
public:
    glfw();
    ~glfw();

    std::vector<monitor> get_monitors() const;
    monitor get_primary_monitor() const;

    window create_window(int width, int height, const char* title) const;
    void window_hint(int target, int hint) const;

    void poll_events() const;

private:
    glfw(const glfw&) = delete;
    glfw& operator = (const glfw&) = delete;
};

}
}
}

#endif
