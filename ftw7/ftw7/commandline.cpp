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
#include "argp/argp.h"
#include "commandline.hpp"

const char* argp_program_version = "ftw7 0.0.1";
const char* argp_program_bug_address = "/dev/null";
static const char doc[] = "Fullscreen Textmode Demo Viewer for Windows 7";
static const char args_doc[] = "DEMO-EXECUTABLE";
static const struct argp argp = { nullptr, nullptr, args_doc, doc };

namespace ftw7
{

void parse_command_line(int argc, char* argv[])
{
    argp_parse(&argp, argc, argv, 0, nullptr, nullptr);
}

}
