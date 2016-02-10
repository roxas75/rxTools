twl_p9 = 0x8014A00;

SECTIONS
{
	. = twl_p9 + 0x38C0;
	.patch.p9.sig : { *(.patch.p9.sig) }
	
	. = twl_p9 + 0x13656;
	.patch.p9.sha_compare_stub : { *(.patch.p9.sha_compare_stub) }
	
	. = twl_p9 + 0x13164;
	.patch.p9.whitelist_check_disable : { *(.patch.p9.whitelist_check_disable) }
	
	. = twl_p9 + 0x1355E;
	.patch.p9.cartridge_save_type_check_disable : { *(.patch.p9.cartridge_save_type_check_disable) }
	
	. = twl_p9 + 0x1356A;
	.patch.p9.dsi_cartridge_save_exploit_check_disable : { *(.patch.p9.dsi_cartridge_save_exploit_check_disable) }
	
	. = twl_p9 + 0x13002;
	.patch.p9.ninlogo_check_disable : { *(.patch.p9.ninlogo_check_disable) }
	
	. = twl_p9 + 0x13552;
	.patch.p9.blacklist_check_disable : { *(.patch.p9.blacklist_check_disable) }
}
