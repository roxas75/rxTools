SECTIONS
{
	. = payload;
	.text : { *(.text) *(.text*) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }
}
