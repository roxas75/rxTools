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

#include <stdint.h>
#include <wchar.h>
#include <memory.h>
#include <svc.h>
#include <process9.h>

_Noreturn void __attribute__((section(".patch.p9.reboot.entry")))
loadExecReboot(int r0, int r1, int r2, uint32_t hiId, uint32_t loId)
{
	wchar_t path[64];
	size_t written;
	uintptr_t reboot;
	P9File f;

	swprintf(path, sizeof(path), L"sdmc:/rxTools/data/%08X%08X.bin", hiId, loId);
	p9Open(f, path, 1);
	p9Read(f, &written, (void *)FIRM_ADDR, 67108864);

	while (p9RecvPxi() == 0x44846);
	svcKernelSetState(SVC_KERNEL_STATE_INIT, hiId, loId,
		SVC_KERNEL_STATE_TITLE_COMPAT);

#ifdef PLATFORM_KTR
	reboot = 0x0817F4FC;
#else
	reboot = 0x080FF4FC;
#endif
	svcBackdoor((void *)reboot);
	__builtin_unreachable();
}
