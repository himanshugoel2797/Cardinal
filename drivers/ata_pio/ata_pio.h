#ifndef _ATA_PIO_DRIVER_H_
#define _ATA_PIO_DRIVER_H_

#include "types.h"
#include "drivers.h"
#include "managers.h"

uint8_t ATA_PIO_Initialize();

uint8_t ATA_PIO_ReadStatus(bool preempt);

void ATA_PIO_SelectDrive(uint8_t disk);
uint8_t ATA_PIO_Identify(uint16_t *result);
uint8_t ATA_PIO_Write(uint64_t addr, uint16_t *data, uint16_t sectorCount);
uint8_t ATA_PIO_Read(uint64_t addr, uint16_t *data, uint16_t sectorCount);
void ATA_PIO_Reset();

#endif