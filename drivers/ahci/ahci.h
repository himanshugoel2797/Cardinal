#ifndef _ACHI_DRIVER_H_
#define _ACHI_DRIVER_H_

#include "types.h"
#include "drivers.h"
#include "priv_ahci.h"

/**
 * \defgroup ahci AHCI Driver
 * @{
*/

//! Initialize the AHCI controller

//! \return 0 on success, anything else on failure
uint8_t
AHCI_Initialize(void);

//! Reset the AHCI controller

void
AHCI_Reset(void);

//! Check the device type of the current port

//! \return the AHCI device type
uint8_t
AHCI_CheckDeviceType(HBA_PORT *port);

//! Read from the disk

//! \param start The LBA48 sector start address
//! \param count The number of bytes to copy over, this is rounded up to a multiple of 512
//! \param buf The buffer to put the read data into
//! \return TRUE on read success, FALSE on failure
bool
AHCI_Read(HBA_PORT *port,
          uint64_t start,
          uint32_t count,
          uint16_t *buf);

//! Read from the first disk

//! \param start The LBA48 sector start address
//! \param count The number of bytes to copy over, this is rounded up to a multiple of 32
//! \param buf The buffer to put the read data into
//! \return TRUE on read success, FALSe on failure
bool
AHCI_0_Read(uint64_t start,
            uint32_t count,
            uint16_t *buf);



//! Write to the disk

//! \param start The LBA48 sector start address
//! \param count The number of bytes to write, this is rounded down to a multiple of 512
//! \param buf The buffer to read data from
//! \return TRUE on write success, FALSE on failure
bool
AHCI_Write(HBA_PORT *port,
           uint64_t start,
           uint32_t count,
           uint16_t *buf);


//! Write to the first disk

//! \param start The LBA48 sector start address
//! \param count The number of bytes to write, this is rounded down to a multiple of 512
//! \param buf The buffer to read data from
//! \return TRUE on write success, FALSE on failure
bool
AHCI_0_Write(uint64_t start,
             uint32_t count,
             uint16_t *buf);


/**@}*/

#endif