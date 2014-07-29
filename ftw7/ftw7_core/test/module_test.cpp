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
#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/unit_test.hpp>
#include "ftw7_core/wexcept.hpp"
#include "ftw7_core/windows/module.hpp"
#include "ftw7_core/windows/windows_error.hpp"
#include "check_what.hpp"

namespace
{

using namespace ftw7_core::windows;

BOOST_AUTO_TEST_SUITE(module_test)

BOOST_AUTO_TEST_CASE(get_module_filename_test)
{
    // Calling process' .exe file:
    // Hard to test, since we need to know our own exe's location, which is a
    // problem in itself. Simply check whether the path ends with the right
    // filename.
    BOOST_CHECK(boost::algorithm::ends_with(
        get_module_filename(nullptr), L"\\ftw7_core_test.exe"));

    // Another module. Use kernel32.dll, since that is guaranteed to exist.
    // Also simply check whether the path ends with the right filename.
    // Getting the full path right is non-trivial. We don't know where Windows
    // is installed, and on 64 bit systems we'd also have to distinguish between
    // native and WoW64 DLLs.
    BOOST_CHECK(boost::algorithm::ends_with(
        get_module_filename(LoadLibraryW(L"kernel32.dll")), L"\\kernel32.dll"));

    // Invalid module handle.
    BOOST_CHECK_EXCEPTION(get_module_filename((HMODULE)-1),
        ftw7_core::wruntime_error, check_wwhat(
            L"could not obtain module file name of module FFFFFFFF: "
            L"The specified module could not be found."));
}

BOOST_AUTO_TEST_CASE(get_module_handle_test)
{
    BOOST_CHECK(boost::algorithm::ends_with(
        get_module_filename(get_module_handle(nullptr)),
        L"\\ftw7_core_test.exe"));

    BOOST_CHECK_EQUAL(get_module_handle(L"kernel32.dll"),
        LoadLibraryW(L"kernel32.dll"));

    BOOST_CHECK_EXCEPTION(get_module_handle(L"not_loaded.dll"),
        windows_error, check_wwhat(
            L"could not obtain module handle of `not_loaded.dll': "
            L"The specified module could not be found."));
}

BOOST_AUTO_TEST_CASE(get_proc_address_test)
{
    HMODULE kernel32 = LoadLibraryW(L"kernel32.dll");

    BOOST_CHECK_EQUAL(get_proc_address(kernel32, "ExitProcess"),
        GetProcAddress(kernel32, "ExitProcess"));

    BOOST_CHECK_EXCEPTION(get_proc_address(nullptr, "does_not_exist"),
        windows_error, check_wwhat(
            L"could not obtain address of `does_not_exist' from module 00000000: "
            L"The specified procedure could not be found."));
}

BOOST_AUTO_TEST_SUITE_END()

}
