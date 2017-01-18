/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
