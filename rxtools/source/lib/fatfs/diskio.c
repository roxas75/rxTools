/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include <stdbool.h>
#include "diskio.h"     /* FatFs lower layer API */
#include <lib/tmio.h>
#include <lib/fs.h>
/* Definitions of physical drive number for each media */
enum {
	DRV_SDMC,
	DRV_NAND,
	DRV_EMU,

	DRV_NUM
};

static bool initedTmio = false;
static bool initedCrypto = false;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber (0..) */
)
{
	uint32_t res;

	if (initedTmio == false) {
		tmio_init();
		initedTmio = true;
	}

	if ((pdrv == DRV_EMU || pdrv == DRV_NAND) && initedCrypto == false) {
		FSNandInitCrypto();
		initedCrypto = true;
	}

	switch (pdrv) {
		case DRV_EMU:
		case DRV_SDMC:
			res = tmio_init_sdmc();
			break;

		case DRV_NAND:
			res = tmio_init_nand();
			break;

		default:
			return RES_PARERR;
	}

	return res ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber (0..) */
)
{
	enum tmio_dev_id d;

	if ((pdrv == DRV_EMU || pdrv == DRV_NAND) && initedCrypto == false)
		return STA_NOINIT;

	switch (pdrv) {
		case DRV_EMU:
		case DRV_SDMC:
			d = TMIO_DEV_SDMC;
			break;

		case DRV_NAND:
			d = TMIO_DEV_SDMC;
			break;

		default:
			return RES_PARERR;
	}

	return tmio_dev[d].total_size > 0 ? RES_OK : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address (LBA) */
    UINT count      /* Number of sectors to read (1..128) */
)
{
    switch(pdrv){
        case DRV_SDMC:
            if (tmio_readsectors(TMIO_DEV_SDMC, sector,count,(uint8_t *)buff))
                return RES_PARERR;
            break;
        case DRV_NAND:
            nand_readsectors(sector, count, (uint8_t *)buff, CTRNAND);
            break;
        case DRV_EMU:
            emunand_readsectors(sector, count, (uint8_t *)buff, CTRNAND);
            break;
    }
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber (0..) */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address (LBA) */
    UINT count          /* Number of sectors to write (1..128) */
)
{
    switch(pdrv){
        case DRV_SDMC:
            if (tmio_writesectors(TMIO_DEV_SDMC, sector,count,(uint8_t *)buff))
                return RES_PARERR;
            break;
        case DRV_NAND:
            nand_writesectors(sector, count, (uint8_t *)buff, CTRNAND);
			break;
        case DRV_EMU:
            emunand_writesectors(sector, count, (uint8_t *)buff, CTRNAND);
			break;
    }
    return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
	return RES_OK;
    //return RES_PARERR; // Stubbed
}
#endif
