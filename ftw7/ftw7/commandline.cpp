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
#include <algorithm>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "argp/argp.h"
#include "commandline.hpp"
#include "ftw7_core/log/log.hpp"
#include "ftw7_core/windows/string.hpp"
#include "ftw7_core/wexcept.hpp"
#include "ftw7_version.h"

const char* argp_program_version = PACKAGE_STRING;
const char* argp_program_bug_address = PACKAGE_BUGREPORT;

namespace ftw7
{
namespace
{

using ftw7_core::windows::multibyte_to_wstring;
using ftw7_core::windows::wstring_to_multibyte;

enum
{
    OPT_SCREEN_HEIGHT = 'h',
    OPT_SCREEN_WIDTH = 'w',
    force_the_following_options_out_of_the_ascii_range = 256,
    OPT_DRIVER,
    OPT_FULLSCREEN,
    OPT_LIST_DISPLAYS,
    OPT_LOG_LEVEL,
    OPT_NO_ERROR_DIALOGS,
    OPT_NO_WAIT,
    OPT_REFRESH_RATE,
    OPT_SEPARATE_CONSOLE,
};

const char doc[] = "Fullscreen Textmode Demo Viewer for Windows 7";
const char args_doc[] = "DEMO [-- DEMO COMMAND LINE ARGUMENTS]";

void parse_log_level(const char* arg, const argp_state* state, command_line_arguments& args)
{
    using namespace ftw7_core::log;

    auto found_level = std::find_if(log_level_info_begin(),
        log_level_info_end(),
        [&](const log_level_info& li) { return !strcmp(arg, li.console_display_name); });

    if (found_level == log_level_info_end())
    {
        argp_failure(state, EXIT_FAILURE, 0, "`%s' is not a valid log level", arg);
    }
    else
    {
        args.demo_settings.emulation_settings.log_level = found_level->level;
    }
}

template <typename T>
void parse_non_negative_number(T& target, const char* arg, const argp_state* state, const char* description)
{
    try
    {
        target = boost::lexical_cast<T>(arg);
        if (target >= 0)
        {
            return;
        }
    }
    catch (const boost::bad_lexical_cast&) {}
    argp_failure(state, EXIT_FAILURE, 0, "bad %s `%s'", description, arg);
}

ftw7_core::emulation::display_driver_code parse_driver(const char* arg, const argp_state* state)
{
    if (!strcmp(arg, "gdi"))
    {
        return ftw7_core::emulation::display_driver_code::gdi;
    }
    else if (!strcmp(arg, "opengl"))
    {
        return ftw7_core::emulation::display_driver_code::opengl;
    }
    else
    {
        argp_failure(state, EXIT_FAILURE, 0, "bad driver `%s'", arg);
        return ftw7_core::emulation::display_driver_code::gdi;
    }
}

error_t parse_option(int key, const char* arg, const argp_state* state)
{
    auto& args = *static_cast<command_line_arguments*>(state->input);
    switch (key)
    {
    case ARGP_KEY_ARG:
        args.demo_command_line.push_back(multibyte_to_wstring(arg));
        return 0;
    case ARGP_KEY_END:
        if (state->arg_num < 1)
        {
            argp_error(state, "No demo given");
        }
        return 0;
    case OPT_DRIVER:
        args.demo_settings.emulation_settings.display_driver_code = parse_driver(arg, state);
        return 0;
    case OPT_FULLSCREEN:
        args.demo_settings.emulation_settings.fullscreen = true;
        return 0;
    case OPT_LIST_DISPLAYS:
        args.action = action::list_displays;
        return 0;
    case OPT_LOG_LEVEL:
        parse_log_level(arg, state, args);
        return 0;
    case OPT_NO_ERROR_DIALOGS:
        args.demo_settings.emulation_settings.no_error_dialogs = true;
        return 0;
    case OPT_NO_WAIT:
        args.demo_settings.wait_for_process = false;
        return 0;
    case OPT_REFRESH_RATE:
        parse_non_negative_number(args.demo_settings.emulation_settings.refresh_rate, arg, state, "refresh rate");
        return 0;
    case OPT_SCREEN_HEIGHT:
        parse_non_negative_number(args.demo_settings.emulation_settings.screen_height, arg, state, "screen height");
        return 0;
    case OPT_SCREEN_WIDTH:
        parse_non_negative_number(args.demo_settings.emulation_settings.screen_width, arg, state, "screen width");
        return 0;
    case OPT_SEPARATE_CONSOLE:
        args.demo_settings.separate_console = true;
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }
}

error_t parse_option_stub(int key, char* arg, argp_state* state)
{
    // Don't let exceptions propagate into argp.
    try
    {
        return parse_option(key, arg, state);
    }
    catch (const ftw7_core::wruntime_error& e)
    {
        argp_failure(state, EXIT_FAILURE, 0, wstring_to_multibyte(CP_ACP, e.wwhat()).c_str());
    }
    catch (const std::exception& e)
    {
        argp_failure(state, EXIT_FAILURE, 0, e.what());
    }
    throw std::logic_error("calling argp_failure did not exit the program");
}

std::string create_log_level_doc()
{
    using namespace ftw7_core::log;
    std::string d("Specify log level. Valid log levels are ");

    for (auto i = log_level_info_begin(); i != log_level_info_end(); ++i)
    {
        if (i != log_level_info_begin())
        {
            d += ", ";
        }
        d += '`';
        d += i->console_display_name;
        d += '\'';
    }
    d += '.';

    return doc;
}

}

command_line_arguments parse_command_line(int argc, char* argv[])
{
    // argp's help_filter mechanism is horrible. Among other things it requires the
    // use of malloc. So we rather create dynamic argument documentation up-front,
    // at the expense of using CPU time and memory to create and store these strings
    // even if they might not be required because the help is not displayed.
    const auto log_level_doc = create_log_level_doc();

    const argp_option options[] =
    {
        { 0, 0, 0, 0, "Info options" },
        { "list-displays", OPT_LIST_DISPLAYS, 0, 0, "List active display adapters and their monitors and exit" },
        { 0, 0, 0, 0, "Display options" },
        { "driver", OPT_DRIVER, "driver", 0, "Specify display driver. Available drivers are `gdi' and `opengl' (default)" },
        { "fullscreen", OPT_FULLSCREEN, 0, 0, "Run in fullscreen mode" },
        { "refresh-rate", OPT_REFRESH_RATE, "rate", 0, "Display refresh rate for fullscreen mode" },
        { "screen-width", OPT_SCREEN_WIDTH, "width", 0, "Screen width for fullscreen mode" },
        { "screen-height", OPT_SCREEN_HEIGHT, "height", 0, "Screen height for fullscreen mode" },
        { 0, 0, 0, 0, "Options mainly useful for development" },
        { "separate-console", OPT_SEPARATE_CONSOLE, 0, 0, "Run demo with a separate console window" },
        { 0, 0, 0, 0, "Miscellaneous options" },
        { "log-level", OPT_LOG_LEVEL, "level", 0, log_level_doc.c_str() },
        { "no-error-dialogs", OPT_NO_ERROR_DIALOGS, 0, 0, "Do not show OS error dialogs on demo crash" },
        { "no-wait", OPT_NO_WAIT, 0, 0, "Do not wait for the demo to terminate" },
        { 0 }
    };
    const argp argp = { options, parse_option_stub, args_doc, doc };

    command_line_arguments args;
    argp_parse(&argp, argc, argv, 0, nullptr, &args);
    return args;
}

}
