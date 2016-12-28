#ifndef _CARDINAL_AHCI_DRIVER_H_
#define _CARDINAL_AHCI_DRIVER_H_

#include <cardinal/cardinal_types.h>

typedef enum {
    ATA_CMD_READ_DMA_EXT = 0x25
} ATA_CMD;

typedef enum {
    FISType_RegisterH2D = 0x27,
    FISType_RegisterD2H = 0x34,
    FISType_DMAActivate = 0x39,
    FISType_DMASetup = 0x41,
    FISType_Data = 0x46,
} FISType;

typedef struct {
    uint8_t fisType;
    struct {
        uint8_t pm_port : 4;
        uint8_t rsv0 : 3;
        uint8_t cmd : 1;
    };
    uint8_t command;
    uint8_t features_lo;
    uint16_t lba_lo;
    uint8_t lba_mid;
    uint8_t device;
    uint16_t lba_mid_h;
    uint8_t lba_hi;
    uint8_t features_hi;
    uint16_t count;
    uint8_t icc;
    uint8_t control;
    uint32_t rsv1;
} RegisterH2D_FIS;

typedef struct {

} RegisterD2H_FIS;

typedef struct {

} DMASetupFIS;

typedef struct {

} PIOSetupFIS;

typedef struct {

} Device2HostFIS;

typedef struct {

} SDBsFIS;

typedef struct {
    DMASetupFIS dma;
    uint8_t rsv0[8];
    PIOSetupFIS ps;
    uint8_t rsv1[12];
    Device2HostFIS d2h;
    uint8_t rsv2[4];
    SDBsFIS sdbs;
} FISLayout;

typedef struct {
    uint16_t cfl : 5;
    uint16_t atapi : 1;
    uint16_t write : 1;
    uint16_t prefetch : 1;
    uint16_t reset : 1;
    uint16_t bist : 1;
    uint16_t dc : 6;
} AHCICommand_Info;

typedef struct {
    AHCICommand_Info info;
    uint16_t PRDTL;
    uint32_t byteCount;
    uint32_t commandTableBaseAddress;
    uint32_t commandTableBaseAddressUpper;
    uint32_t rsv[4];
} AHCICommand;

typedef struct {
    uint32_t baseAddress;
    uint32_t baseAddressUpper;
    uint32_t rsv0;
    union {
        uint32_t byteCount : 22;
        uint32_t rsv1 : 9;
        uint32_t intCompletion : 1;
    };
} AHCI_PRDT;

typedef struct {
    uint8_t cmd_fis[64];
    uint8_t atapiCMD[16];
    uint8_t rsv[48];
    AHCI_PRDT prdt[128];
} AHCICommandTable;

#endif