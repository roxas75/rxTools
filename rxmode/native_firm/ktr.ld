payload = 0x0801A4C0;
INCLUDE payload.ld

rebootRam = 0x0817F604;
ENTRY(rebootRam)

SECTIONS
{
	. = 0x0801B3D4;
	.patch.k9.flags : { *(.patch.k9.flags) }

	. = process9 + 0x35498;
	.patch.p9.sig.1 : { *(.patch.p9.sig.1) }

	. = process9 + 0x3B374;
	.patch.p9.sig.2 : { *(.patch.p9.sig.2) }

	. = process9 + 0x5077E;
	.patch.p9.nand.write : { *(.patch.p9.nand.write) }

	. = process9 + 0x507BE;
	.patch.p9.nand.read : { *(.patch.p9.nand.read) }

	. = process9 + 0x5D958;
	.patch.p9.reboot.entry : {
		*(.patch.p9.reboot.entry.top)
		*(.patch.p9.reboot.entry)
	}

	. = process9 + 0x5E0B0;
	.patch.p9.thread : { *(.patch.p9.thread) }

	. = reboot;
	.patch.p9.reboot.body : {
		*(.patch.p9.reboot.body.text.start)
		*(.patch.p9.reboot.body*)
	}

	. = codeSetHook;
	.patch.k11.codeset.hook : { *(.patch.k11.codeset.hook) }

	. = 0x1ff824d8;
	.patch.k11.codeset.addr : { *(.patch.k11.codeset.addr) }
}
