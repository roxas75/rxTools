SECTIONS
{
	.text : { *(.text) *(.text*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }

	. = 0x080238c0;
	.patch.p9.sig : { *(.patch.p9.sig) }
}
