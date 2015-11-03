#ifndef _ACPI_TABLES_DRIVER_H_
#define _ACPI_TABLES_DRIVER_H_

#include "types.h"

/**
 * \defgroup acpi_tables ACPI Tables
 *@{
 */

#define MADT_SIG "APIC"	//!< MADT
#define FADT_SIG "FACP"	//!< FADT
#define HPET_SIG "HPET"	//!< HPET Table
#define MCFG_SIG "MCFG" //!< MCFG Table

//! Initialize the ACPITables driver

//! \return ACPI version on success, 0xFF on failure
uint8_t
ACPITables_Initialize(void);

//! Search for an ACPI table with the given name

//! \param table_name The name of the table to search for
//! \param index The instance of the table to search for
//! \return The physical address of the table on success, NULL on failure
void*
ACPITables_FindTable(const char *table_name,
                     int index);

/**@}*/

#endif /* end of include guard: _ACPI_TABLES_DRIVER_H_ */
