@ Signature/hash patch found by Steveice10, see https://gist.github.com/Steveice10/8a6965c59192ac970832
@ This stubs the function commonly used to compare SHA hashes to always succeed. It's at least used to check SRL (DSi titles) signatures and SHA hashes in general.

.section .patch.p9.sha_compare_stub, "a"
.thumb
	mov		r0, #1
	bx		lr
.pool
