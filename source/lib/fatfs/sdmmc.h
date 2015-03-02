#ifndef __SDMMC_H__
#define __SDMMC_H__

#include <stdint.h>

#define SDMMC_BASE  0x10006000

#define REG_SDSTATUS0   0x1c
#define REG_SDSTATUS1   0x1e
#define REG_SDRESET     0xe0
#define REG_SDCLKCTL    0x24
#define REG_SDOPT       0x28
#define REG_SDCMD       0x00
#define REG_SDCMDARG    0x04
#define REG_SDCMDARG0   0x04
#define REG_SDCMDARG1   0x06
#define REG_SDSTOP      0x08
#define REG_SDRESP      0x0c

#define REG_SDRESP0     0x0c
#define REG_SDRESP1     0x0e
#define REG_SDRESP2     0x10
#define REG_SDRESP3     0x12
#define REG_SDRESP4     0x14
#define REG_SDRESP5     0x16
#define REG_SDRESP6     0x18
#define REG_SDRESP7     0x1a

#define REG_SDBLKLEN    0x26
#define REG_SDBLKCOUNT  0x0a
#define REG_SDFIFO      0x30

#ifdef __cplusplus
extern "C" {
#endif

/*int sdmmc_sdcard_init();
void sdmmc_sdcard_readsector(uint32_t sector_no, void *out);
void sdmmc_sdcard_readsectors(uint32_t sector_no, uint32_t numsectors, void *out);
void sdmmc_sdcard_writesector(uint32_t sector_no, void *in);
void sdmmc_sdcard_writesectors(uint32_t sector_no, uint32_t numsectors, void *in);
void sdmmc_blktransferinit();*/

void sdmmc_sdcard_init();
int sdmmc_sdcard_readsector(uint32_t sector_no, uint8_t *out);
int sdmmc_sdcard_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out);
int sdmmc_sdcard_writesector(uint32_t sector_no, uint8_t *in);
int sdmmc_sdcard_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *in);

int sdmmc_nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out);
int sdmmc_nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *in);

void InitSD();
int Nand_Init();
int SD_Init();

#ifdef __cplusplus
};
#endif

//---------------------------------------------------------------------------------
static inline uint16_t sdmmc_read16(uint16_t reg) {
//---------------------------------------------------------------------------------
    return *(volatile uint16_t*)(SDMMC_BASE + reg);
}

//---------------------------------------------------------------------------------
static inline void sdmmc_write16(uint16_t reg, uint16_t val) {
//---------------------------------------------------------------------------------
    *(volatile uint16_t*)(SDMMC_BASE + reg) = val;
}

//---------------------------------------------------------------------------------
static inline void sdmmc_mask16(uint16_t reg, uint16_t clear, uint16_t set) {
//---------------------------------------------------------------------------------
    uint16_t val = sdmmc_read16(reg);
    val &= ~clear;
    val |= set;
    sdmmc_write16(reg, val);
}

static inline void setckl(uint32_t data)
{
    sdmmc_mask16(REG_SDCLKCTL,0x100,0);
    sdmmc_mask16(REG_SDCLKCTL,0x2FF,data&0x2FF);
    sdmmc_mask16(REG_SDCLKCTL,0x0,0x100);
}

#endif
