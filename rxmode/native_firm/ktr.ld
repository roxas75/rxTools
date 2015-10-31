payload = 0x0801A4C0;
INCLUDE payload.ld

SECTIONS
{
	. = 0x0801B3D4;
	.patch.k9.flags : { *(.patch.k9.flags) }

	. = process9 + 0x5D958;
	.patch.p9.reboot.entry : { *(.patch.p9.reboot.entry) }

	. = process9 + 0x35498;
	.patch.p9.sig.1 : { *(.patch.p9.sig.1) }

	. = process9 + 0x3B374;
	.patch.p9.sig.2 : { *(.patch.p9.sig.2) }

	. = process9 + 0x5077E;
	.patch.p9.nand.write : { *(.patch.p9.nand.write) }

	. = process9 + 0x507BE;
	.patch.p9.nand.read : { *(.patch.p9.nand.read) }

	. = process9 + 0x5E0B0;
	.patch.p9.mythread.ldr : { *(.patch.p9.mythread.ldr) }

	. = process9 + 0x5E0E4;
	.patch.p9.mythread.addr : { *(.patch.p9.mythread.addr) }

	. = reboot;
	.patch.p9.reboot.body : { *(.patch.p9.reboot.body) }

	. = codeSetHook;
	.patch.k11.codeset.hook : { *(.patch.k11.codeset.hook) }

	. = 0x1ff824d8;
	.patch.k11.codeset.addr : { *(.patch.k11.codeset.addr) }
}
