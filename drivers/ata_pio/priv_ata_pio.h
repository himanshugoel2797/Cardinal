#ifndef _PRIV_ATA_PIO_DRIVER_H_
#define _PRIV_ATA_PIO_DRIVER_H_

#include "types.h"
#include "utils/native.h"
#include "managers.h"
#include "drivers.h"

#define IDE_CTRL_A_BASE 0x1F0
#define IDE_CTRL_B_BASE 0x170

#define IDE_MASTER 0xA0
#define IDE_SLAVE 0xB0

#define STATUS_BUSY 0x80
#define STATUS_DRQ 0x08
#define STATUS_ERR 0x01

#define DATA_PORT(x) ((x+0))
#define FEAT_ERR_PORT(x) ((x+1))
#define SECTOR_COUNT_PORT(x) ((x+2))
#define LBA_LO_PORT(x) ((x+3))
#define LBA_MID_PORT(x) ((x+4))
#define LBA_HI_PORT(x) ((x+5))
#define DRIVE_PORT(x) ((x+6))	//Disk Select
#define CMD_REG_STATUS_PORT(x) ((x+7))
#define SPECIAL_STATUS_PORT(x) (x + 0x206)

#define DRIVE_DIAG_CMD 0x90
#define FORMAT_TRACK_CMD 0x50
#define IDENTIFY_DRIVE_CMD 0xEC
#define INITIALIZE_DRIVE_CMD 0x91
#define NOP_CMD 0x00
#define READ_BUFFER_CMD 0xE4
#define READ_BUFFER_DMA_R_CMD 0xC8
#define READ_BUFFER_DMA_CMD 0xC9
#define READ_LONG_R_CMD 0x22
#define READ_LONG_CMD 0x23
#define READ_MULTIPLE_CMD 0xC4
#define READ_SECTORS_R_CMD 0x20
#define READ_SECTORS_CMD 0x21
#define READ_SECTORS_EXT 0x24
#define READ_VER_SECTORS_R_CMD 0x40
#define READ_VER_SECTORS_CMD 0x41
#define RECALIBRATE_CMD 0x10
#define SEEK_CMD 0x70
#define SET_FEATURES_CMD 0xEF
#define SET_MULTIPLE_MODE_CMD 0xC6
#define WRITE_BUFFER_CMD 0xE8
#define WRITE_DMA_R_CMD 0xCA
#define WRITE_DMA_CMD 0xCB
#define WRITE_LONG_R_CMD 0x32
#define WRITE_LONG_CMD 0x33
#define WRITE_SECTORS_EXT 0x34
#define WRITE_MULTIPLE_CMD 0xC5
#define WRITE_SAME_CMD 0xE9
#define WRITE_SECTORS_R_CMD 0x30
#define WRITE_SECTORS_CMD 0x31
#define WRITE_VERIFY_CMD 0x3C

#define CHECK_PWR_MODE_CMD 0x98
#define IDLE_CMD 0x97
#define IDLE_IMM_CMD 0x95
#define SLEEP_CMD 0x99
#define STANDBY_CMD 0x96
#define STANDBY_IMM_CMD 0x94

#endif