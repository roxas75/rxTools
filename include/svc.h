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

typedef enum {
	SVC_KERNEL_STATE_INIT = 0
} SvcKernelState;

enum {
	SVC_KERNEL_STATE_TITLE_COMPAT = 0
};

static inline _Noreturn svcExitThread()
{
	__asm__ volatile ("svc 9\n");
	__builtin_unreachable();
}

static inline void svcBackdoor(void (* p)())
{
	register void (* r0) __asm__("r0");

	r0 = p;
	__asm__ volatile ("svc 123\n" :: "r"(r0));
}

static inline int svcKernelSetState(SvcKernelState state,
	int param0, int param1, int param2)
{
	register SvcKernelState r0 __asm__("r0");
	register int r1 __asm__("r1");
	register int r2 __asm__("r2");
	register int r3 __asm__("r3");

	register int res __asm__("r0");

	r0 = state;
	r1 = param0;
	r2 = param1;
	r3 = param2;

	__asm__ volatile ("svc 124\n"
		: "=r"(res) : "r"(r0), "r"(r1), "r"(r2), "r"(r3));

	return res;
}
