#include "ata_pio.h"
#include "priv_ata_pio.h"
#include "utils/native.h"

uint16_t curBase;
uint8_t curDisk;

uint8_t ATA_PIO_Initialize() {
    curBase = IDE_CTRL_A_BASE;
}

uint8_t ATA_PIO_ReadStatus(bool preempt) {
    return inb(preempt? CMD_REG_STATUS_PORT(curBase) : SPECIAL_STATUS_PORT(curBase));
}

void ATA_PIO_SelectDrive(uint8_t disk) {
    curDisk = disk;
    outb(DRIVE_PORT(curBase), disk);

}

uint8_t ATA_PIO_Identify(uint16_t *result) {
    //Check if the bus even exists
    if(inb(CMD_REG_STATUS_PORT(curBase)) == 0xFF)return -1;	//The bus doesn't exist

    //IDENTIFY the disk
    outb(SECTOR_COUNT_PORT(curBase), 0);

    //Higher bytes first!
    outb(LBA_HI_PORT(curBase), 0);
    outb(LBA_MID_PORT(curBase), 0);
    outb(LBA_LO_PORT(curBase), 0);

    outb(CMD_REG_STATUS_PORT(curBase), IDENTIFY_DRIVE_CMD);

    uint8_t status = ATA_PIO_ReadStatus(TRUE);
    if(status == 0)return -1;
    while(status & STATUS_BUSY != STATUS_BUSY)status = ATA_PIO_ReadStatus(TRUE);	//Keep polling until the busy bit clears

    uint8_t lba_mid = inb(LBA_MID_PORT(curBase));
    uint8_t lba_hi = inb(LBA_HI_PORT(curBase));

    if(lba_mid != 0 | lba_hi != 0)return -1;

    while(1) {
        status = ATA_PIO_ReadStatus(TRUE);
        if(status & STATUS_DRQ == STATUS_DRQ)break;
        if(status & STATUS_ERR == STATUS_ERR)return -1;
    }

    //Read in 256 words and store them into the provided pointer
    for(int i = 0; i < 256; i++) {
        result[i] = inl(DATA_PORT(curBase));
    }

    return 0;
}

uint8_t ATA_PIO_Write(uint64_t addr, uint16_t *data, uint16_t sectorCount) {
    outb(DRIVE_PORT(curBase), 0x40 | ((curDisk == IDE_SLAVE) << 4));

    outb(FEAT_ERR_PORT(curBase), 0x00);
    outb(SECTOR_COUNT_PORT(curBase), (sectorCount >> 8) & 0xFF);
    outb(LBA_LO_PORT(curBase), (addr >> 24) & 0xFF);
    outb(LBA_MID_PORT(curBase), (addr >> 32) & 0xFF);
    outb(LBA_HI_PORT(curBase), (addr >> 40) & 0xFF);

    outb(FEAT_ERR_PORT(curBase), 0x00);
    outb(SECTOR_COUNT_PORT(curBase), sectorCount & 0xFF);
    outb(LBA_LO_PORT(curBase), (addr) & 0xFF);
    outb(LBA_MID_PORT(curBase), (addr >> 8) & 0xFF);
    outb(LBA_HI_PORT(curBase), (addr >> 16) & 0xFF);

    outb(CMD_REG_STATUS_PORT(curBase), WRITE_SECTORS_EXT);

    uint32_t written_data;

    //TODO we now need to queue the callback along with the source pointer
    while(!(ATA_PIO_ReadStatus(TRUE) & STATUS_DRQ)) {
        if(ATA_PIO_ReadStatus(TRUE) & STATUS_DRQ == 0)break;
        //ThreadMan_Yield();	//Keep yielding to the other threads until the disk is ready
    }

    if(ATA_PIO_ReadStatus(TRUE) & STATUS_BUSY == 1) {
        //The busy bit is frozen reset disk
        ATA_PIO_Reset();
        return -1;
    }



}

uint8_t ATA_PIO_Read(uint64_t addr, uint16_t *data, uint16_t sectorCount) {
    outb(DRIVE_PORT(curBase), 0x40 | ((curDisk == IDE_SLAVE) << 4));

    outb(FEAT_ERR_PORT(curBase), 0x00);
    outb(SECTOR_COUNT_PORT(curBase), (sectorCount >> 8) & 0xFF);
    outb(LBA_LO_PORT(curBase), (addr >> 24) & 0xFF);
    outb(LBA_MID_PORT(curBase), (addr >> 32) & 0xFF);
    outb(LBA_HI_PORT(curBase), (addr >> 40) & 0xFF);

    outb(FEAT_ERR_PORT(curBase), 0x00);
    outb(SECTOR_COUNT_PORT(curBase), sectorCount & 0xFF);
    outb(LBA_LO_PORT(curBase), (addr) & 0xFF);
    outb(LBA_MID_PORT(curBase), (addr >> 8) & 0xFF);
    outb(LBA_HI_PORT(curBase), (addr >> 16) & 0xFF);

    outb(CMD_REG_STATUS_PORT(curBase), READ_SECTORS_EXT);

    //TODO queue the callback and the destination pointer for processing
}

void ATA_PIO_Reset() {
    outb(SPECIAL_STATUS_PORT(curBase), 2);
    for(int i = 0; i < 5000; i++);
    outb(SPECIAL_STATUS_PORT(curBase), 0);

    if(curDisk != IDE_MASTER) {
        ATA_PIO_SelectDrive(curDisk);
    }
}