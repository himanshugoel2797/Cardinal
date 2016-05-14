#ifndef _PRIV_IO_APIC_DRIVER_H_
#define _PRIV_IO_APIC_DRIVER_H_

#include "types.h"

/**
 * \defgroup ioapic_driver IO APIC driver
 * @{
*/

//! Read an IOAPIC configuration register

//! \param io_apic_baseAddr the base address of the IO APIC MMIO space
//! \param index the register index to read
//! \return the value of the register
uint32_t
IOAPIC_Read(uint32_t* io_apic_baseAddr,
            uint32_t index);

//! Write an IOAPIC configuration register

//! \param io_apic_baseAddr the base address of the IO APIC MMIO space
//! \param index the register index to read
//! \param val the register value
void
IOAPIC_Write(uint32_t* io_apic_baseAddr,
             uint32_t index,
             uint32_t val);


typedef struct {
    uint32_t* baseAddr;
    uint32_t global_int_base;
    uint32_t ID;
    uint32_t entry_count;
} IOAPIC_Desc;

typedef struct {
    uint32_t ioapic_index;
    uint32_t ioapic_pin;
} IOAPIC_InterruptMapEntry;

/**@}*/

#endif /* end of include guard: _PRIV_IO_APIC_DRIVER_H_ */
