/*
 * Copyright 2012-2014 Thomas Mathys
 *
 * This file is part of ftw7.
 *
 * ftw7 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ftw7 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ftw7.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ARGP_COMPAT_H_INCLUDED
#define ARGP_COMPAT_H_INCLUDED

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#if defined(HAVE_MEMPCPY) && !HAVE_MEMPCPY
void* argp_compat_mempcpy(void* out, const void* in, size_t n);
#define mempcpy argp_compat_mempcpy
#endif

#if defined(HAVE_SLEEP) && !HAVE_SLEEP
void argp_compat_sleep(unsigned int seconds);
#define sleep argp_compat_sleep
#endif

#if defined(HAVE_STRCASECMP) && !HAVE_STRCASECMP
int argp_compat_strcasecmp(const char* s1, const char* s2);
#define strcasecmp argp_compat_strcasecmp
#endif

#if defined(HAVE_STRCHRNUL) && !HAVE_STRCHRNUL
const char* argp_compat_strchrnul(const char* s, int c);
#define strchrnul argp_compat_strchrnul
#endif

#if defined(HAVE_STRNDUP) && !HAVE_STRNDUP
char* argp_compat_strndup(const char* s, size_t n);
#define strndup argp_compat_strndup
#endif

#endif
