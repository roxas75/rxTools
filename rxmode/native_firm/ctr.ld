payload = 0x0801A4C0;
INCLUDE payload.ld

SECTIONS
{
	. = 0x0801B564;
	.patch.k9.flags : { *(.patch.k9.flags) }

	. = process9 + 0x002F8;
	.patch.p9.keyx : { *(.patch.p9.keyx) }

	. = process9 + 0x3563C;
	.patch.p9.sig.1 : { *(.patch.p9.sig.1) }

	. = process9 + 0x3B2B8;
	.patch.p9.sig.2 : { *(.patch.p9.sig.2) }

	. = process9 + 0x5082C;
	.patch.p9.nand.write : { *(.patch.p9.nand.write) }

	. = process9 + 0x5086C;
	.patch.p9.nand.read : { *(.patch.p9.nand.read) }

	. = process9 + 0x5E140;
	.patch.p9.mythread.ldr : { *(.patch.p9.mythread.ldr) }

	. = process9 + 0x5E174;
	.patch.p9.mythread.addr : { *(.patch.p9.mythread.addr) }

	. = process9 + 0x5D9EC;
	.patch.p9.reboot.entry : { *(.patch.p9.reboot.entry) }

	. = reboot;
	.patch.p9.reboot.body : { *(.patch.p9.reboot.body) }

	. = codeSetHook;
	.patch.k11.codeset.hook : { *(.patch.k11.codeset.hook) }

	. = 0x1ff824f0;
	.patch.k11.codeset.addr : { *(.patch.k11.codeset.addr) }
}
