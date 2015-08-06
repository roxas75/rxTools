SECTIONS
{
	. = 0x0801a4c0;
	.text : { *(.text) *(.text*) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data : { *(.data) *(.data*) }
	.bss : { *(.bss) *(.bss*) }

	. = 0x0801b3d4;
	.patch.k9.flags : { *(.patch.k9.flags) }

	. = 0x080288f8;
	.patch.p9.keyx : { *(.patch.p9.keyx) }

	. = 0x0805e524;
	.patch.p9.sig.1 : { *(.patch.p9.sig.1) }

	. = 0x080630b0;
	.patch.p9.sig.2 : { *(.patch.p9.sig.2) }

	. = 0x08078d7e;
	.patch.p9.nand.write : { *(.patch.p9.nand.write) }

	. = 0x08078dbe;
	.patch.p9.nand.read : { *(.patch.p9.nand.read) }

	. = 0x080866b0;
	.patch.p9.mythread.ldr : { *(.patch.p9.mythread.ldr) }

	. = 0x080866e4;
	.patch.p9.mythread.addr : { *(.patch.p9.mythread.addr) }

	. = 0x080859ec;
	.patch.p9.reboot.entry : { *(.patch.p9.reboot.entry) }

	. = 0x08094454;
	.patch.p9.reboot.body : { *(.patch.p9.reboot.body) }
}
