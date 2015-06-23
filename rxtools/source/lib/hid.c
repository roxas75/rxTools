#include "hid.h"

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