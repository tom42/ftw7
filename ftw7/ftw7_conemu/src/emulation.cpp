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
#include "ftw7_conemu/display/gdi_display_driver.hpp"
#include "ftw7_conemu/emulation/emulation.hpp"
#include "ftw7_core/log/log.hpp"
#include "ftw7_core/mhookpp/mhookpp.hpp"
#include "ftw7_core/windows/module.hpp"

namespace ftw7_conemu
{
namespace emulation
{
namespace
{

// TODO: decide where to stash all of this
typedef decltype(WriteConsoleOutputA)* WriteConsoleOutputA_ptr_t;
WriteConsoleOutputA_ptr_t TrueWriteConsoleOutputA;

BOOL WINAPI MyWriteConsoleOutputA(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT)
{
    // TODO: trace this
    // TODO: catch all exceptions (a dispatcher would be nice for this)
    // TODO: throw stuff at the driver
    // TODO: consider putting these functions out of any namespace, just to get
    //       the namespace out of them when logging...
    return TRUE;
}

void install_hooks()
{
    using ftw7_core::windows::get_module_handle;
    using ftw7_core::windows::get_proc_address;
    using ftw7_core::mhookpp::set_hook;

    // TODO: decide how to respond to errors here.
    // Most likely we'll have to intercept stuff from kernel32 and possibly user32 only.
    // These should be always around, so throwing if any of them cannot be loaded is probably totally OK.
    const auto kernel32 = get_module_handle(L"kernel32");

    // TODO: find out how to handle errors here.
    // In theory, get_proc_address might fail because we're attempting to hook a function
    // that simply does not exist on a particular version of windows...
    // TODO: possibly need to dive up hooking into an early stage where
    //       we hook a small subset of functions only (e.g. WriteConsoleA and WriteConsoleW,
    //       those we need for logging). We can then hook those, which should definitely
    //       be around. We can then go on set up the logging already with the hooks in place
    //       and know that if some logging code uses WriteConsoleOutputA it can use the true version of it)
    TrueWriteConsoleOutputA = reinterpret_cast<WriteConsoleOutputA_ptr_t>(get_proc_address(kernel32, "WriteConsoleOutputA"));
    set_hook(&TrueWriteConsoleOutputA, MyWriteConsoleOutputA);
}

}

void initialize(HINSTANCE emulation_dll_module_handle)
{
    install_hooks();
    // TODO: need to put away the display driver somewhere.
    new display::gdi_display_driver(emulation_dll_module_handle);
}

}
}
