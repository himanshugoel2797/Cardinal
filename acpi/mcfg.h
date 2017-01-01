/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _MCFG_ACPI_H_
#define _MCFG_ACPI_H_

#include "types.h"
#include "acpi_tables.h"
#include "priv_acpi_tables.h"

/**
 * \addtogroup acpi_tables ACPI Tables
 * @{
*/

//! An entry in the MCFG table
typedef struct {
    uint64_t baseAddr;
    uint16_t group_segment_number;
    uint8_t start_bus_number;
    uint8_t end_bus_number;
    uint32_t res0;
} MCFG_Entry;

//! The MCFG table
typedef struct {
    ACPISDTHeader h;
    uint64_t res0;
    MCFG_Entry entries[1];
} MCFG;

/**@}*/

#endif /* end of include guard: _MCFG_ACPI_H_ */
