@ Blacklist check patch found by TuxSH (see https://gist.github.com/Steveice10/8a6965c59192ac970832).
@ Disables the cartridge blacklist check (mostly, if not entirely, demo carts). This check was introduced in firmware 4.4.0-10.

.section .patch.p9.blacklist_check_disable, "a"
.thumb
	mov		r0, #1
	nop
.pool
