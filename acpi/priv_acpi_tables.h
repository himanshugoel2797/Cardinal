/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PRIV_ACPI_TABLES_DRIVER_H_
#define _PRIV_ACPI_TABLES_DRIVER_H_

#include "types.h"

/**
 * \addtogroup acpi_tables ACPI Tables
 * @{
*/

//! RSDT pointer Table
typedef struct {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__((packed)) RSDPDescriptor;

//! XSDT pointer Table
typedef struct {
    RSDPDescriptor firstPart;

    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__((packed)) RSDPDescriptor20;

#define RSDP_EXPECTED_SIG "RSD PTR "
#define ACPI_VERSION_1 0
#define ACPI_VERSION_2 2

//! ACPI Section Descriptor Table header
typedef struct {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed)) ACPISDTHeader;

//! XSDT Table
typedef struct {
    ACPISDTHeader h;
    uint64_t PointerToOtherSDT[1];
} __attribute__((packed)) XSDT;

//! RSDT Table
typedef struct {
    ACPISDTHeader h;
    uint32_t PointerToOtherSDT[1];
} __attribute__((packed)) RSDT;

//! Generic ACPI Address structure
typedef struct {
    uint8_t address_space_id; // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed)) GenericAddressStructure;

#define XSDT_GET_POINTER_COUNT(h) ((h.Length - sizeof(h)) / sizeof(uint64_t))
#define RSDT_GET_POINTER_COUNT(h) ((h.Length - sizeof(h)) / sizeof(uint32_t))

bool
ACPITables_ValidateChecksum(ACPISDTHeader *header);

/**@}*/

#endif /* end of include guard: _PRIV_ACPI_TABLES_DRIVER_H_ */
