SECTIONS
{
	.text : { *(.text) *(.text*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }

	. = 0x080238a8;
	.patch.p9.sig : { *(.patch.p9.sig) }
}
