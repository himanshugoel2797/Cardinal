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
	const char *VendorName;
	GenericAddressStructure Address;
	uint8_t HPETNumber;
	uint16_t MinimumTick;
	uint8_t PageProtection;
} __attribute__((packed)) HPET;

#endif