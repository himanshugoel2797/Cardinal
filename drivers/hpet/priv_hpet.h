#ifndef _PRIV_HPET_DRIVER_H_
#define _PRIV_HPET_DRIVER_H_

#include "types.h"

#include "acpi_tables/priv_acpi_tables.h"

typedef struct {
    ACPISDTHeader h;
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    pci_vendor_t pci_vendor_id;
    GenericAddressStructure address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} HPET;

HPET *hpet;
uint64_t frequency;
uint64_t capabilities;

#define HPET_CAP_REG 0x00   //Capabilities register offset
#define HPET_GC_REG 0x10    //General Configuration register offset
#define HPET_GIS_REG 0x20   //General Interrupt Status register offset
#define HPET_MCV_REG 0x0F0  //Main Counter Value register offset

#define TIMER_CONFIG_OFFSET(N) ((0x100 + 0x20 * N) - (0x107 + 0x20 * N))
#define TIMER_COMP_VAL_OFFSET(N) ((0x108 + 0x20 * N) - (0x10F + 0x20 * N))
#define TIMER_FSB_ROUTE_OFFSET(N) ((0x110 + 0x20 * N) - (0x117 + 0x20 * N))

//#define TIMER_CONFIG_OFFSET(N) ((0x20 + (4*N)) + 0x108)
//#define TIMER_COMP_VAL_OFFSET(N) ((0x21 + (4*N)) + 0x10F)
//#define TIMER_FSB_ROUTE_OFFSET(N) ((0x110 + 0x20 * N) - (0x117 + 0x20 * N))


void HPET_Write(uint32_t reg, uint64_t val);
uint64_t HPET_Read(uint32_t reg);

#endif /* end of include guard: _PRIV_HPET_DRIVER_H_ */
