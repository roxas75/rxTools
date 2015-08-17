/*
* Copyright (C) 2015 The PASTA Team
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef LOG_H
#define LOG_H

typedef enum {
	ll_fatal = 0,
	ll_error,
	ll_info,
	ll_warn,
	ll_trace,
	ll_debug
} loglevel_t;

void set_loglevel(loglevel_t ll);
void log_(loglevel_t ll, const char* file, int line, const char* fmt, ...);

#define log(ll, fmt, ...) log_(ll, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif //LOG_H
