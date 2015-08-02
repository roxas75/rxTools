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

#include "hid.h"
#include "screenshot.h"

u32 InputWait() {
    u32 pad_state_old = HID_STATE;
    while (true) {
        u32 pad_state = HID_STATE;
        if (pad_state ^ pad_state_old)
            return ~pad_state;
    }
}

u32 GetInput() {
    u32 pad_state = HID_STATE;
    return ~pad_state;
}

void WaitForButton(u32 button){
	while (true) {
		TryScreenShot(); //Maybe someone wanna take screenshots while waiting
        u32 pad_state = InputWait();
        if (pad_state & button)
            return;
    }
}
