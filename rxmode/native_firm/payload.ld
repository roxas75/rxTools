SECTIONS
{
	. = payload;
	.text : { *(.text) *(.text*) }
	.rodata.keyx : { *(.rodata.keyx) }
	.rodata.label : { *(.rodata.label) }
	.rodata.nand.sector : { *(.rodata.nand.sector) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }
}
