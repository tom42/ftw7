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
#include "GLFW/glfw3.h" // TODO: can we get rid of this?
#include "ftw7_version.h"
#include "ftw7_conemu/display/opengl_display_driver.hpp"
#include "ftw7_conemu/display/vga8x8.hpp"
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

// TODO: not every driver should supply its own palette
const GLfloat palette[] =
{
    0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f, 1,
    0x00 / 255.0f, 0x00 / 255.0f, 0xaa / 255.0f, 1,
    0x00 / 255.0f, 0xaa / 255.0f, 0x00 / 255.0f, 1,
    0x00 / 255.0f, 0xaa / 255.0f, 0xaa / 255.0f, 1,
    0xaa / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f, 1,
    0xaa / 255.0f, 0x00 / 255.0f, 0xaa / 255.0f, 1,
    0xaa / 255.0f, 0x55 / 255.0f, 0x00 / 255.0f, 1,
    0xaa / 255.0f, 0xaa / 255.0f, 0xaa / 255.0f, 1,
    0x55 / 255.0f, 0x55 / 255.0f, 0x55 / 255.0f, 1,
    0x55 / 255.0f, 0x55 / 255.0f, 0xff / 255.0f, 1,
    0x55 / 255.0f, 0xff / 255.0f, 0x55 / 255.0f, 1,
    0x55 / 255.0f, 0xff / 255.0f, 0xff / 255.0f, 1,
    0xff / 255.0f, 0x55 / 255.0f, 0x55 / 255.0f, 1,
    0xff / 255.0f, 0x55 / 255.0f, 0xff / 255.0f, 1,
    0xff / 255.0f, 0xff / 255.0f, 0x55 / 255.0f, 1,
    0xff / 255.0f, 0xff / 255.0f, 0xff / 255.0f, 1,
};

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// TODO: make this a class member to get access to glfw object
glfw::monitor find_monitor(const glfw::glfw& glfw, const wchar_t* display_name)
{
    if (!display_name)
    {
        return glfw.get_primary_monitor();
    }

    for (auto monitor : glfw.get_monitors())
    {
        if (!wcscmp(monitor.display_name(), display_name))
        {
            return monitor;
        }
    }

    // TODO: throw something useful (some error message: display not found: 'display_name');
    throw "yikes";
}

// TODO: make this a class member to get access to the glfw object
glfw::window create_window(const glfw::glfw& glfw, HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    glfw::window window;
    if (settings.fullscreen)
    {
        auto monitor = find_monitor(glfw, LR"(\\.\DISPLAY2)"); // TODO: use display name from settings
        if (settings.refresh_rate)
        {
            glfw.window_hint(GLFW_REFRESH_RATE, settings.refresh_rate);
        }
        window = glfw.create_window(settings.screen_width, settings.screen_height, FTW7_OPENGL_DISPLAY_DRIVER_NAME, &monitor);
        window.input_mode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        window = glfw.create_window(settings.window_width, settings.window_height, FTW7_OPENGL_DISPLAY_DRIVER_NAME, nullptr);
    }        

    // GLFW's mechanism of setting the icon doesn't work, since it assumes that
    // the Window has been created in an EXE rather than a DLL and uses GetModuleHandle(NULL)
    // as the first argument to LoadIconW.
    auto icon = LoadIconW(emulation_dll_module_handle, MAKEINTRESOURCEW(IDI_FTW7));
    SendMessageW(window.win32_window(), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
    SendMessageW(window.win32_window(), WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));

    window.make_context_current();
    // TODO: make this possible again
    //glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    // TODO: here be nastiness for bnz:
    // * No resources are cleaned up
    // * No error codes are checked
    // * Texture dimensions and whatnot are hardcoded
    const GLsizei char_width = 8;
    const GLsizei char_height = 8;
    const GLsizei texture_width = 16 * char_width;
    const GLsizei texture_height = 16 * char_height;
    unsigned char texture_data[texture_width * texture_height * 4];
    const unsigned char* src = vga8x8;
    for (size_t row = 0; row < 16; ++row)
    {
        for (size_t column = 0; column < 16; ++column)
        {
            unsigned char* texel = texture_data + (row * char_height * texture_width + column * char_width) * 4;
            for (size_t y = 0; y < char_height; ++y)
            {
                unsigned char byte = *src++;
                for (size_t x = 0; x < char_width; ++x)
                {
                    if (byte & 128)
                    {
                        // Foreground color
                        *texel++ = 255;
                        *texel++ = 255;
                        *texel++ = 255;
                        *texel++ = 255;
                    }
                    else
                    {
                        // Background color
                        *texel++ = 0;
                        *texel++ = 0;
                        *texel++ = 0;
                        *texel++ = 0;
                    }
                    byte <<= 1;
                }
                texel += (texture_width - char_width) * 4;
            }
        }
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    window.swap_buffers();
    return window;
}

}

opengl_display_driver::opengl_display_driver(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
    : m_window(nullptr) // TODO: can we remove this??? If not, make it so it can be removed?
{
    m_window = create_window(m_glfw, emulation_dll_module_handle, settings);
}

bool opengl_display_driver::handle_messages()
{
    m_glfw.poll_events();
    return !m_window.should_close();
}

void opengl_display_driver::render(const CHAR_INFO* buffer)
{
    // TODO: more nastyness for bnz (hardcoded shite and whatnot, and all in immediate mode since that's all I ever learnt)

    auto fbsize = m_window.framebuffer_size();
    glViewport(0, 0, fbsize.first, fbsize.second);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: this is all hackery that assumes a 4:3 resolution.
    // TODO: this really needs fixage, since it looks totally shit in windowed mode at 640x400
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 480, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, (480 - 400) / 2, 0);

    // 1st pass: background colors
    const CHAR_INFO* p = buffer;
    glBegin(GL_QUADS);
    for (int row = 0; row < 50; ++row)
    {
        for (int column = 0; column < 80; ++column)
        {
            size_t bgcolor_index = (p->Attributes >> 4) & 15;
            glColor4fv(&palette[bgcolor_index * 4]);
            glVertex2i(column * 8 + 8, row * 8 + 0);
            glVertex2i(column * 8 + 0, row * 8 + 0);
            glVertex2i(column * 8 + 0, row * 8 + 8);
            glVertex2i(column * 8 + 8, row * 8 + 8);
            ++p;
        }
    }
    glEnd();

    // 2nd pass: foreground colors
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    p = buffer;
    for (int row = 0; row < 50; ++row)
    {
        for (int column = 0; column < 80; ++column)
        {
            size_t fgcolor_index = p->Attributes & 15;

            // c = character code.
            const size_t c = static_cast<unsigned char>(p->Char.AsciiChar);
            const size_t cx = c & 15;
            const size_t cy = c / 16;
            float x0 = (cx * 8) / 128.0f;
            float y0 = (cy * 8) / 128.0f;
            float x1 = (cx * 8 + 8) / 128.0f;
            float y1 = (cy * 8 + 8) / 128.0f;

            glColor4fv(&palette[fgcolor_index * 4]);

            glTexCoord2f(x1, y0);
            glVertex2i(column * 8 + 8, row * 8 + 0);

            glTexCoord2f(x0, y0);
            glVertex2i(column * 8 + 0, row * 8 + 0);

            glTexCoord2f(x0, y1);
            glVertex2i(column * 8 + 0, row * 8 + 8);

            glTexCoord2f(x1, y1);
            glVertex2i(column * 8 + 8, row * 8 + 8);

            ++p;
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    m_window.swap_buffers();
}

void opengl_display_driver::set_title(const wchar_t* title)
{
    // The GLFW documentation states that glfwSetWindowTitle's title argument is an UTF8 encoded string.
    const auto narrow_title = ftw7_core::windows::wstring_to_multibyte(CP_UTF8, title);
    m_window.title(narrow_title.c_str());
}

}
}
