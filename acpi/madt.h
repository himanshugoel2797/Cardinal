#ifndef _MADT_ACPI_TABLE_H_
#define _MADT_ACPI_TABLE_H_

#include "types.h"
#include "priv_acpi_tables.h"

/**
 * \addtogroup acpi_tables ACPI Tables
 * @{
*/

//! The MADT
typedef struct {
    ACPISDTHeader h;
    uint32_t local_controller_addr;
    uint32_t flags;
    uint8_t entries[1];
} MADT;

//! Header for an entry in the MADT
typedef struct {
    uint8_t type;
    uint8_t entry_size;
} MADT_EntryHeader;

//! A Local APIC entry in the MADT
typedef struct {
    MADT_EntryHeader h;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} MADT_EntryLAPIC;
#define MADT_LAPIC_ENTRY_TYPE 0     //!< The type ID describing a Local APIC entry

//! An IO APIC entry in the MADT
typedef struct {
    MADT_EntryHeader h;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_base_addr;
    uint32_t global_sys_int_base;
} MADT_EntryIOAPIC;
#define MADT_IOAPIC_ENTRY_TYPE 1    //!< The type ID describing an IO APIC entry

//! An ISA override entry in the MADT
typedef struct {
    MADT_EntryHeader h;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t global_sys_int;
    uint16_t flags;
} MADT_EntryISAOVR;
#define MADT_ISAOVER_ENTRY_TYPE 2   //!< The type ID describing an ISA override entry

//! An IO APIC Non-Maskable Interrupt mapping entry in the MADT
typedef struct {
    MADT_EntryHeader h;
    uint16_t flags;
    uint32_t global_sys_int;
} MADT_EntryIOAPIC_NMI;
#define MADT_IOAPIC_NMI_ENTRY_TYPE 3    //!< The type ID describing an IO APIC NMI mapping entry

//! A Local APIC Non-Maskable Interrupt mapping entry in the MADT
typedef struct {
    MADT_EntryHeader h;
    uint8_t apic_id;
    uint16_t flags;
    uint8_t lapic_lint;
} MADT_EntryAPIC_NMI;
#define MADT_APIC_NMI_ENTRY_TYPE 4      //!< The type ID describing a Local APIC NMI mapping entry

/**@}*/

#endif /* end of include guard: _MADT_ACPI_TABLE_H_ */
