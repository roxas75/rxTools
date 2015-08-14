@---------------------------------------------------------------------------------
@	Patch file format
@
@	0	4	Header - 'PPOR'
@	4	4	offset to index
@
@	index entry
@	0	4	Firmware revision this patch set applies to
@	8	4	Offset in file to patch set
@
@	patch set
@	0	4	Header	- 'PTCH'
@	4	4	Number of patches
@
@	for each patch
@	0	4	size of patch
@	4	4	offset in user settings to patch
@	8	...	patch data
@---------------------------------------------------------------------------------

	.arm
	.text

	.global	_start
@---------------------------------------------------------------------------------
_start:
	.word	0x524f5050
	.word	index

#define	P4_LOAD_ADDRESS		0x00279400
#define	P4_S2_LOAD_ADDRESS	0x002B0000

#define	P6_LOAD_ADDRESS		0x00297000
#define	P6_S2_LOAD_ADDRESS	0x00290000
#define	P6_DATAADR			0x00287000


@---------------------------------------------------------------------------------
patch_m4x_4x_description:
@---------------------------------------------------------------------------------
	.asciz	"4.x    for rxTools"

@---------------------------------------------------------------------------------
patch_m4x_4x_1:
@---------------------------------------------------------------------------------

	.word	m4x_4x_1_end - m4x_4x_1_start
	.word	0

@---------------------------------------------------------------------------------
m4x_4x_1_start: @ 0x279400
@---------------------------------------------------------------------------------
	.word 0x0010F2B9  @ pop_r0_r2
	.word P4_LOAD_ADDRESS + (m4x_4x_1_YS - m4x_4x_1_start)
	.word 0xBADC0DED  @ r2
	.word 0x0018F19C  @ mount_sd("YS:") -> LDMFD           SP!, {R3-R5,PC}
	.word 0x0010B690  @ r3
	.word 0x00FAB000  @ r4
	.word 0x00200200  @ r5
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word 0x00000001  @ r2
	.word 0x001549E1  @ pop_r1
	.word 0x00295E0C  @ r1 Offset for the launcher path string
	.word 0x001B82AC  @ ifile_open(0x00279000, path, 1) -> LDMFD   SP!, {R4-R8,PC}
	.word 0x0018D5DC  @ r4
	.word 0x00278340  @ r5
	.word 0x00100200  @ r6
	.word 0x000048CC  @ r7
	.word 0x00143D60  @ r8
	.word 0x001549E1  @ pop_r1
	.word 0x00004000  @ r1 2nd stage offset in file
	.word 0x00143D8C  @ pop_r0
	.word 0x00279004  @ r0 handle.offset
	.word 0x0010CCBC  @ str r1, [r0] -> LDMFD   SP!, {R4,PC}
	.word 0x44444444  @ r4
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word P4_S2_LOAD_ADDRESS
	.word 0x0010538C  @ pop_r3
	.word 0xDEADBEEF  @ r3 this(or similar junk pops) has to be here for patch_count1
	.word 0x0010538C  @ pop_r3
	.word 0x00000400  @ r3
	.word 0x001549E1  @ pop_r1
	.word 0x00279020  @ r1
	.word 0x001B3958  @ ifile_read(0x00279000, 0x00279020, 0x002B0000, 0x00000400) -> LDMFD   SP!, {R4-R9,PC}
	.word 0x002104E5  @ r4
	.word 0x0019DA00  @ r5
	.word 0x00017500  @ r6
	.word 0x0021DF86  @ r7
	.word 0x001AC100  @ r8
	.word 0x001DDA22  @ r9
	.word 0x0010538C  @ pop_r3
	.word (P4_S2_LOAD_ADDRESS - (P4_LOAD_ADDRESS + m4x_4x_1_post_pivot - m4x_4x_1_start))
	.word 0x00143D60  @ pivot -> ADD SP, SP, R3        LDR PC, [SP],#4
m4x_4x_1_post_pivot:
m4x_4x_1_YS:
	.word 0x003A5359  @ YS:
@---------------------------------------------------------------------------------
m4x_4x_1_end:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
patch_m4x_dg_description:
@---------------------------------------------------------------------------------
	.asciz	"4.x DG for rxTools"
    
@---------------------------------------------------------------------------------
patch_m4x_ndg_description:
@---------------------------------------------------------------------------------
	.asciz	"4.x N3DS DG for rxTools"

@---------------------------------------------------------------------------------
patch_m4x_dg_1:
@---------------------------------------------------------------------------------

	.word	m4x_dg_1_end - m4x_dg_1_start
	.word	0

@---------------------------------------------------------------------------------
m4x_dg_1_start: @ 0x279400
@---------------------------------------------------------------------------------
	.word 0x0010F2B9  @ pop_r0_r2
	.word P4_LOAD_ADDRESS + (m4x_dg_1_YS - m4x_dg_1_start)
	.word 0xBADC0DED  @ r2
	.word 0x0018F19C  @ mount_sd("YS:") -> LDMFD           SP!, {R3-R5,PC}
	.word 0x0010B690  @ r3
	.word 0x00FAB000  @ r4
	.word 0x00200200  @ r5
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word 0x00000001  @ r2
	.word 0x001549E1  @ pop_r1
	.word 0x00295E0C  @ r1 Offset for the launcher path string
	.word 0x001B82AC  @ ifile_open(0x00279000, path, 1) -> LDMFD   SP!, {R4-R8,PC}
	.word 0x0018D5DC  @ r4
	.word 0x00278340  @ r5
	.word 0x00100200  @ r6
	.word 0x000048CC  @ r7
	.word 0x00143D60  @ r8
	.word 0x001549E1  @ pop_r1
	.word 0x00004400  @ r1 2nd stage offset in file
	.word 0x00143D8C  @ pop_r0
	.word 0x00279004  @ r0 handle.offset
	.word 0x0010CCBC  @ str r1, [r0] -> LDMFD   SP!, {R4,PC}
	.word 0x44444444  @ r4
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word P4_S2_LOAD_ADDRESS
	.word 0x0010538C  @ pop_r3
	.word 0xDEADBEEF  @ r3 this(or similar junk pops) has to be here for patch_count1
	.word 0x0010538C  @ pop_r3
	.word 0x00000400  @ r3
	.word 0x001549E1  @ pop_r1
	.word 0x00279020  @ r1
	.word 0x001B3958  @ ifile_read(0x00279000, 0x00279020, 0x002B0000, 0x00000400) -> LDMFD   SP!, {R4-R9,PC}
	.word 0x002104E5  @ r4
	.word 0x0019DA00  @ r5
	.word 0x00017500  @ r6
	.word 0x0021DF86  @ r7
	.word 0x001AC100  @ r8
	.word 0x001DDA22  @ r9
	.word 0x0010538C  @ pop_r3
	.word (P4_S2_LOAD_ADDRESS - (P4_LOAD_ADDRESS + m4x_dg_1_post_pivot - m4x_dg_1_start))
	.word 0x00143D60  @ pivot -> ADD SP, SP, R3        LDR PC, [SP],#4
m4x_dg_1_post_pivot:
m4x_dg_1_YS:
	.word 0x003A5359  @ YS:
@---------------------------------------------------------------------------------
m4x_dg_1_end:
@---------------------------------------------------------------------------------


@---------------------------------------------------------------------------------
patch_m4x_ndg_1:
@---------------------------------------------------------------------------------

	.word	m4x_ndg_1_end - m4x_ndg_1_start
	.word	0

@---------------------------------------------------------------------------------
m4x_ndg_1_start: @ 0x279400
@---------------------------------------------------------------------------------
	.word 0x0010F2B9  @ pop_r0_r2
	.word P4_LOAD_ADDRESS + (m4x_ndg_1_YS - m4x_ndg_1_start)
	.word 0xBADC0DED  @ r2
	.word 0x0018F19C  @ mount_sd("YS:") -> LDMFD           SP!, {R3-R5,PC}
	.word 0x0010B690  @ r3
	.word 0x00FAB000  @ r4
	.word 0x00200200  @ r5
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word 0x00000001  @ r2
	.word 0x001549E1  @ pop_r1
	.word 0x00295E0C  @ r1 Offset for the launcher path string
	.word 0x001B82AC  @ ifile_open(0x00279000, path, 1) -> LDMFD   SP!, {R4-R8,PC}
	.word 0x0018D5DC  @ r4
	.word 0x00278340  @ r5
	.word 0x00100200  @ r6
	.word 0x000048CC  @ r7
	.word 0x00143D60  @ r8
	.word 0x001549E1  @ pop_r1
	.word 0x00004800  @ r1 2nd stage offset in file
	.word 0x00143D8C  @ pop_r0
	.word 0x00279004  @ r0 handle.offset
	.word 0x0010CCBC  @ str r1, [r0] -> LDMFD   SP!, {R4,PC}
	.word 0x44444444  @ r4
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x00279000  @ r0
	.word P4_S2_LOAD_ADDRESS
	.word 0x0010538C  @ pop_r3
	.word 0xDEADBEEF  @ r3 this(or similar junk pops) has to be here for patch_count1
	.word 0x0010538C  @ pop_r3
	.word 0x00000400  @ r3
	.word 0x001549E1  @ pop_r1
	.word 0x00279020  @ r1
	.word 0x001B3958  @ ifile_read(0x00279000, 0x00279020, 0x002B0000, 0x00000400) -> LDMFD   SP!, {R4-R9,PC}
	.word 0x002104E5  @ r4
	.word 0x0019DA00  @ r5
	.word 0x00017500  @ r6
	.word 0x0021DF86  @ r7
	.word 0x001AC100  @ r8
	.word 0x001DDA22  @ r9
	.word 0x0010538C  @ pop_r3
	.word (P4_S2_LOAD_ADDRESS - (P4_LOAD_ADDRESS + m4x_ndg_1_post_pivot - m4x_ndg_1_start))
	.word 0x00143D60  @ pivot -> ADD SP, SP, R3        LDR PC, [SP],#4
m4x_ndg_1_post_pivot:
m4x_ndg_1_YS:
	.word 0x003A5359  @ YS:
@---------------------------------------------------------------------------------
m4x_ndg_1_end:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
patch_m4x_2:
@---------------------------------------------------------------------------------
	.word	m4x_2_end - m4x_2_start
	.word	0x1b4

@---------------------------------------------------------------------------------
m4x_2_start:
@---------------------------------------------------------------------------------
	.word 0x0010F2B9  @ pop_r0_r2
	.word 0x0001FE00  @ r0
	.word 0x00000100  @ r2

	.word 0x001C4FC4  @ pop_r1
	.word P4_LOAD_ADDRESS
	
	.word 0x001334FC  @ eeprom_read(0x0001FE00, 0x00279400, 0x00000100) -> LDMFD           SP!, {R4,PC}
	.word 0xFFFFFFFE  @ r4
	
	.word 0x0010538C  @ pop_r3
	.word P4_LOAD_ADDRESS - 0x0FFFFF64
	.word 0x00143D60 @ add sp,sp,r3 | ldr pc,[sp],#4
@---------------------------------------------------------------------------------
m4x_2_end:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
patch_m6x_6x_description:
@---------------------------------------------------------------------------------
	.asciz	"6.x    for rxTools"

@---------------------------------------------------------------------------------
patch_m6x_6x_1:
@---------------------------------------------------------------------------------
	.word	m6x_6x_end - m6x_6x_start
	.word	0

@---------------------------------------------------------------------------------
m6x_6x_start:
@---------------------------------------------------------------------------------
	.word 0x001A9349 		@ pop	{r0, pc}
	.word 0x00280252 		@ r0 (char* "YS:")
	.word 0x0019B498 		@ FS_MOUNTSDMC()
	.word 0xDEADDEAD 		@ r3 (garbage)
	.word 0xDEADDEAD 		@ r4 (garbage)
	.word 0xDEADDEAD 		@ r5 (garbage)
	.word 0x0011BE4D 		@ pop {r1, r2, r3, pc}
	.word 0x002A4204 		@ r1 Offset for the launcher path string
	.word 0x00000001 		@ r2 (openflags)
	.word 0xDEADDEAD 		@ r3 (garbage)
	.word 0x001A9349 		@ pop	{r0, pc}
	.word P6_DATAADR		@ r0 (_this)
	.word 0x001C08B8 		@ IFile_Open(_this, path, openflags)
	.word 0xDEADDEAD 		@ r4 (garbage)
	.word 0xDEADDEAD 		@ r5 (garbage)
	.word 0xDEADDEAD 		@ r6 (garbage)
	.word 0xDEADDEAD 		@ r7 (garbage)
	.word 0xDEADDEAD 		@ r8 (garbage)
	.word 0x001CD804		@ pop_r1 *****************************
	.word 0x00005000		@ r1 2nd stage offset in file
	.word 0x001A9349		@ pop_r0
	.word P6_DATAADR + 4	@ r0 handle.offset
	.word 0x0010CF5C		@ str r1, [r0] -> LDMFD   SP!, {R4,PC}
	.word 0x44444444		@ r4 *********************************
	.word 0x001A9349		@ pop_r0
	.word 0xDEADDEAD		@ r0 (garbage)
	.word 0x0011BE4D 		@ pop {r1, r2, r3, pc}
	.word 0xDEADDEAD		@ r1 (garbage)
	.word 0xBADC0DED		@ r2 (garbage) - is replaced by counter and CRC1
	.word 0xDEADDEAD		@ r2 (garbage)
	.word 0x0011BE4D 		@ pop {r1, r2, r3, pc}
	.word P6_DATAADR + 32	@ r1 (&readbytes)
	.word P6_S2_LOAD_ADDRESS@ r2 (dst)
	.word 0x00000400 		@ r3 (size)
	.word 0x001A9349 		@ pop	{r0, pc}
	.word P6_DATAADR		@ r0 (_this)
	.word 0x001BC18C 		@ IFile_Read(_this, &readbytes, dst, size)
	.word 0xDEADDEAD 		@ r4 (garbage)
	.word 0xDEADDEAD 		@ r5 (garbage)
	.word 0xDEADDEAD 		@ r6 (garbage)
	.word 0xDEADDEAD 		@ r7 (garbage)
	.word 0xDEADDEAD 		@ r8 (garbage)
	.word 0xDEADDEAD 		@ r9 (garbage)
	.word 0x00144BA4 		@ pop {r3} | add sp, sp, r3 | pop {pc}
	.word (P6_S2_LOAD_ADDRESS - (P6_LOAD_ADDRESS + m6x_6x_end - m6x_6x_start)) @ r3
@---------------------------------------------------------------------------------
m6x_6x_end:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
patch_m6x_2:
@---------------------------------------------------------------------------------
	.word	m6x_2_end - m6x_2_start
	.word	0x1b4

@---------------------------------------------------------------------------------
m6x_2_start:
@---------------------------------------------------------------------------------
	.word 0x00144CF8  @ pop_r0
	.word 0x0001FE00  @ r0
	.word 0x0011BE4D  @ pop_r1_r3
	.word P6_LOAD_ADDRESS
	.word 0x00000100  @ r2
	.word 0x33333333  @ r3
	.word 0x00133838  @ eeprom_read(0x0001FE00, 0x00297000, 0x00000100) -> LDMFD           SP!, {R4,PC}
	.word 0x44444444  @ r4
	.word 0x00144BA4  @ pivot -> LDR R3, [SP],#4           ADD SP, SP, R3        LDR PC, [SP],#4
	.word (P6_LOAD_ADDRESS - 0x0FFFFF64)
@---------------------------------------------------------------------------------
m6x_2_end:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
patch_count1:
@---------------------------------------------------------------------------------
	.word	2
	.word	0x70

	.hword	0x51

@---------------------------------------------------------------------------------
patch_count2:
@---------------------------------------------------------------------------------

	.word	2
	.word	0x170

	.hword	0x52

@---------------------------------------------------------------------------------
patch_message_length:
@---------------------------------------------------------------------------------

	.word	2
	.word	0x150

	.hword	0x6e

@---------------------------------------------------------------------------------
patch_m4x_4x:
@---------------------------------------------------------------------------------
	.word	0x50544348
	.word	5
	.word	patch_m4x_4x_1
	.word	patch_m4x_2
	.word	patch_count1
	.word	patch_count2
	.word	patch_message_length

@---------------------------------------------------------------------------------
patch_m4x_dg:
@---------------------------------------------------------------------------------
	.word	0x50544348
	.word	5
	.word	patch_m4x_dg_1
	.word	patch_m4x_2
	.word	patch_count1
	.word	patch_count2
	.word	patch_message_length

@---------------------------------------------------------------------------------
patch_m4x_ndg:
@---------------------------------------------------------------------------------
	.word	0x50544348
	.word	5
	.word	patch_m4x_ndg_1
	.word	patch_m4x_2
	.word	patch_count1
	.word	patch_count2
	.word	patch_message_length
    
@---------------------------------------------------------------------------------
patch_m6x_6x:
@---------------------------------------------------------------------------------
	.word	0x50544348
	.word	5
	.word	patch_m6x_6x_1
	.word	patch_m6x_2
	.word	patch_count1
	.word	patch_count2
	.word	patch_message_length

@---------------------------------------------------------------------------------
index:
@---------------------------------------------------------------------------------
	.word	patch_m4x_4x_description
	.word	patch_m4x_4x

	.word	patch_m4x_dg_description
	.word	patch_m4x_dg
    
    .word	patch_m4x_ndg_description
	.word	patch_m4x_ndg

	.word	patch_m6x_6x_description
	.word	patch_m6x_6x
