OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(uvl_start)

SECTIONS
{
	. = 0x009D2000;
	.text   : { *(.text.start) *(.text   .text.*   .gnu.linkonce.t.*) *(.sceStub.text.*) }
	.rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
	.data   : { *(.data   .data.*   .gnu.linkonce.d.*) }
	.bss    : { *(.bss    .bss.*    .gnu.linkonce.b.*) *(COMMON) }

	. = 0x18420000;
	.compat : {*(.compat)}
}
