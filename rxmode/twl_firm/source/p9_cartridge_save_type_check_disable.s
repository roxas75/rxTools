@ Cartridge save type check patch found by TuxSH (see https://gist.github.com/Steveice10/8a6965c59192ac970832).
@ This check was introduced firmware 4.4.0-10, and blocked most DSi-compatible flashcarts as a result.

.section .patch.p9.cartridge_save_type_check_disable, "a"
.thumb
	mov		r0, #1
	nop
.pool
