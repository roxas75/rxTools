.arm
.global ioDelay
.type   ioDelay STT_FUNC

@ioDelay ( u32 us )
ioDelay:
	subs r0, #1
	bgt ioDelay
	bx lr