codeSetHookInArm9 = 0x1FFADC00;
codeSetHookInArm11 = 0xFFFF0C00;

SECTIONS
{
	. = 0x0801a4c0;
	.text : { *(.text) *(.text*) }
	.rodata.keyx : { *(.rodata.keyx) }
	.rodata.label : { *(.rodata.label) }
	.rodata.nand.sector : { *(.rodata.nand.sector) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }

	. = 0x0801b3d4;
	.patch.k9.flags : { *(.patch.k9.flags) }

	. = 0x08085958;
	.patch.p9.reboot.entry : { *(.patch.p9.reboot.entry) }

	. = 0x0809435C;
	.patch.p9.reboot.body : { *(.patch.p9.reboot.body) }

	. = 0x0805d498;
	.patch.p9.sig.1 : { *(.patch.p9.sig.1) }

	. = 0x08063374;
	.patch.p9.sig.2 : { *(.patch.p9.sig.2) }

	. = 0x0807877e;
	.patch.p9.nand.write : { *(.patch.p9.nand.write) }

	. = 0x080787be;
	.patch.p9.nand.read : { *(.patch.p9.nand.read) }

	. = 0x080860b0;
	.patch.p9.mythread.ldr : { *(.patch.p9.mythread.ldr) }

	. = 0x080860e4;
	.patch.p9.mythread.addr : { *(.patch.p9.mythread.addr) }

	. = codeSetHookInArm9;
	.patch.k11.codeset.hook : { *(.patch.k11.codeset.hook) }

	. = 0x1ff824d8;
	.patch.k11.codeset.addr : { *(.patch.k11.codeset.addr) }
}
