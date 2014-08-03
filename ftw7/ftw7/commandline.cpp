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
    force_the_following_options_out_of_the_ascii_range = 256,
    OPT_LOG_LEVEL,
    OPT_NO_WAIT,
    OPT_SEPARATE_CONSOLE,
};

const char doc[] = "Fullscreen Textmode Demo Viewer for Windows 7";
const char args_doc[] = "DEMO [-- DEMO COMMAND LINE ARGUMENTS]";

void parse_log_level(char* arg, struct argp_state* state, command_line_arguments& args)
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

error_t parse_option(int key, char* arg, struct argp_state* state)
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
    case OPT_LOG_LEVEL:
        parse_log_level(arg, state, args);
        return 0;
    case OPT_NO_WAIT:
        args.demo_settings.wait_for_process = false;
        return 0;
    case OPT_SEPARATE_CONSOLE:
        args.demo_settings.separate_console = true;
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }
}

error_t parse_option_stub(int key, char* arg, struct argp_state* state)
{
    // Don't let exceptions propagate into argp.
    try
    {
        return parse_option(key, arg, state);
    }
    catch (const ftw7_core::wruntime_error& e)
    {
        argp_failure(state, EXIT_FAILURE, 0, wstring_to_multibyte(e.wwhat()).c_str());
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
    std::string doc("Specify log level. Valid log levels are ");

    for (auto i = log_level_info_begin(); i != log_level_info_end(); ++i)
    {
        if (i != log_level_info_begin())
        {
            doc += ", ";
        }
        doc += '`';
        doc += i->console_display_name;
        doc += '\'';
    }
    doc += '.';

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

    const struct argp_option options[] =
    {
        { 0, 0, 0, 0, "Options mainly useful for development" },
        { "separate-console", OPT_SEPARATE_CONSOLE, 0, 0, "Run demo with a separate console window" },
        { 0, 0, 0, 0, "Miscellaneous options" },
        { "log-level", OPT_LOG_LEVEL, "level", 0, log_level_doc.c_str() },
        { "no-wait", OPT_NO_WAIT, 0, 0, "Do not wait for the demo to terminate" },
        { 0 }
    };
    const struct argp argp = { options, parse_option_stub, args_doc, doc };

    command_line_arguments args;
    argp_parse(&argp, argc, argv, 0, nullptr, &args);
    return args;
}

}
