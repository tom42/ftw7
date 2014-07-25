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
#include <stdexcept>
#include "argp/argp.h"
#include "commandline.hpp"
#include "ftw7_core/string.hpp"
#include "ftw7_core/wexcept.hpp"

const char* argp_program_version = "ftw7 0.0.1";
const char* argp_program_bug_address = "/dev/null";
static const char doc[] = "Fullscreen Textmode Demo Viewer for Windows 7";
static const char args_doc[] = "DEMO";

static const struct argp_option options[] =
{
    {0}
};

namespace ftw7
{
namespace
{

error_t parse_option(int key, char* arg, struct argp_state* state)
{
    auto& args = *static_cast<command_line_arguments*>(state->input);
    switch (key)
    {
    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
        {
            argp_error(state, "More than one demo given");
        }
        args.demo_executable_path = ftw7_core::multibyte_to_wstring(arg);
        return 0;
    case ARGP_KEY_END:
        if (state->arg_num < 1)
        {
            argp_error(state, "No demo given");
        }
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
        argp_failure(state, EXIT_FAILURE, 0, ftw7_core::wstring_to_multibyte(e.wwhat()).c_str());
    }
    catch (const std::exception& e)
    {
        argp_failure(state, EXIT_FAILURE, 0, e.what());
    }
    throw std::logic_error("calling argp_failure did not exit the program");
}

}

command_line_arguments parse_command_line(int argc, char* argv[])
{
    command_line_arguments args;
    static const struct argp argp = { options, parse_option_stub, args_doc, doc };
    argp_parse(&argp, argc, argv, 0, nullptr, &args);
    return args;
}

}
