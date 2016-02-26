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

#include <stdbool.h>
#include "draw.h"
#include "hid.h"
#include "i2c.h"
#include "screenshot.h"

static void bgWork()
{
	TryScreenShot();

	// Check whether HOME or POWER button has been pressed
	if (*(volatile uint8_t *)0x10147021 == 13) {
		fadeOut();
		// Return to HOME menu
		i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
		while (1);
	}
}

uint32_t InputWait() {
    uint32_t pad_state_old = HID_STATE;
    while (true) {
        //bgWork();

        uint32_t pad_state = HID_STATE;
        if (pad_state ^ pad_state_old)
            return ~pad_state;
    }
}

uint32_t GetInput() {
    uint32_t pad_state = HID_STATE;
    return ~pad_state;
}

void WaitForButton(uint32_t button){
	while (true) {
		//bgWork();

        uint32_t pad_state = InputWait();
        if (pad_state & button)
            return;
    }
}
