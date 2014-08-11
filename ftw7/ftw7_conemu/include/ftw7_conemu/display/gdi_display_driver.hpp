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
#ifndef FTW7_CONEMU_DISPLAY_GDI_DISPLAY_DRIVER_HPP_INCLUDED
#define FTW7_CONEMU_DISPLAY_GDI_DISPLAY_DRIVER_HPP_INCLUDED

#include <Windows.h>
#include <cstdint>
#include <vector>
#include "ftw7_conemu/display/display_driver.hpp"
#include "ftw7_core/emulation/emulation.hpp"
#include "ftw7_core/windows/unique_hdc.hpp"
#include "ftw7_core/windows/unique_hwnd.hpp"
#include "ftw7_core/windows/unique_window_class.hpp"

namespace ftw7_conemu
{
namespace display
{

class gdi_display_driver : public display_driver
{
public:
    gdi_display_driver(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings);
    virtual ~gdi_display_driver();
    bool handle_messages() override;
    void render(const CHAR_INFO* buffer) override;
    void set_title(const wchar_t* title) override;
private:
    gdi_display_driver(const gdi_display_driver&) = delete;
    gdi_display_driver& operator = (const gdi_display_driver&) = delete;

    ftw7_core::windows::unique_window_classexw m_wc;
    ftw7_core::windows::unique_hwnd m_hwnd;
    ftw7_core::windows::unique_hdc m_hdc;
    std::vector<uint32_t> m_renderbuffer;
};

}
}

#endif
