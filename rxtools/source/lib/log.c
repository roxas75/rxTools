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

#include "log.h"

#include "fatfs/ff.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG_FILENAME _T("rxTools/rx.log")
#define LOGSTR_LENMAX (512)
loglevel_t loglevel = ll_fatal;
FIL* logfile = NULL;
char prefix[128];
char logstr[LOGSTR_LENMAX];
static const char* loglevel2str[] = {"FATAL", "ERROR", "INFO ", "WARN", "TRACE", "DEBUG"};

void set_loglevel(loglevel_t ll) {
	loglevel = ll;
}

inline void log_open_() {
	logfile = (FIL*)calloc(1, sizeof(FIL));
	if(f_open(logfile, LOG_FILENAME, FA_WRITE | FA_OPEN_EXISTING) == FR_OK) {
		unsigned int sz = f_size(logfile);
		if(sz) f_lseek(logfile, sz);
		return;
	}
	if(f_open(logfile, LOG_FILENAME, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
		f_lseek(logfile, 0);
		return;
	}
	free(logfile);
	logfile = (FIL*)(uintptr_t)1U;
}

void log_(loglevel_t ll, const char* file, int line, const char* fmt, ...) {
	int r;
	unsigned int bw;
	va_list argp;
	if(ll > loglevel) return;
	if(logfile == NULL) log_open_();
	if((uintptr_t)logfile == 1U) return;
	r = snprintf(prefix, 128, "[%s|%s:%d] ", loglevel2str[ll], file, line);
	if(r > 127) r = 127;
	f_write(logfile, prefix, r, &bw);
	va_start(argp, fmt);
	r = vsnprintf(logstr, LOGSTR_LENMAX, fmt, argp);
	if(r > LOGSTR_LENMAX-1) r = LOGSTR_LENMAX-1;
	f_write(logfile, logstr, r, &bw);
	f_write(logfile, "\r\n", 2, &bw);
	f_sync(logfile);
}
