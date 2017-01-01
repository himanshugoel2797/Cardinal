/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _HPET_ACPI_TABLE_H_
#define _HPET_ACPI_TABLE_H_

#include "types.h"
#include "priv_acpi_tables.h"

typedef struct {
    ACPISDTHeader h;
    uint8_t RevisionID;
    uint8_t ComparatorCount : 5;
    uint8_t CounterIs64Bit : 1;
    uint8_t Rsv0 : 1;
    uint8_t LegacyReplacement : 1;
    uint16_t VendorID;
    GenericAddressStructure Address;
    uint8_t HPETNumber;
    uint16_t MinimumTick;
    uint8_t PageProtection;
} __attribute__((packed)) HPET;

#endif