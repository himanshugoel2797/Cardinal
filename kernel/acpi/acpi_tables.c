/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "acpi_tables.h"
#include "priv_acpi_tables.h"
#include "boot_information/boot_information.h"
#include "common.h"
#include "memory.h"

static RSDPDescriptor20 *rsdp;

uint8_t
ACPITables_Initialize(void) {
    rsdp = NULL;
    //Find the RSDP table
    uint8_t *rsdp_sig = (uint8_t*)GetBootInfo()->RSDPAddress;

    if (rsdp_sig[0] == RSDP_EXPECTED_SIG[0]) {
        //Check the full signature
        if (!strncmp((const char*)rsdp_sig, RSDP_EXPECTED_SIG, 8)) {
            rsdp = (RSDPDescriptor20*) rsdp_sig;
            uint32_t checksum = 0;

            for (uint8_t *tmp = rsdp_sig; tmp < rsdp_sig + sizeof(RSDPDescriptor); tmp++) {
                checksum += *tmp;
            }


            if ((checksum & 0xff) == 0) return rsdp->firstPart.Revision;
            else rsdp = NULL;
        }
    }

    return -1;
}

bool
ACPITables_ValidateChecksum(ACPISDTHeader *header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < header->Length; i++) {

        sum += ((char *)header)[i];
    }

    return sum == 0;
}

void*
ACPITables_FindTable(const char *table_name,
                     int index) {
    if (rsdp == NULL) return NULL;


    if(rsdp->firstPart.Revision != ACPI_VERSION_1 && rsdp->XsdtAddress) {
        XSDT *xsdt = (XSDT*)GetVirtualAddress(CachingModeWriteBack, (void*)rsdp->XsdtAddress);
        if (!ACPITables_ValidateChecksum((ACPISDTHeader*)xsdt)) return (void*)-1;

        int entries = XSDT_GET_POINTER_COUNT((xsdt->h));
        int cur_index = 0;

        for (int i = 0; i < entries; i++) {
            if(xsdt->PointerToOtherSDT[i] == 0)continue;
            ACPISDTHeader *h = (ACPISDTHeader *)GetVirtualAddress(CachingModeWriteBack, (void*)xsdt->PointerToOtherSDT[i]);
            if (!strncmp(h->Signature, table_name, 4) && ACPITables_ValidateChecksum(h)) {
                if (cur_index == index)
                    return (void *) h;

                cur_index++;
            }
        }
    } else if ((rsdp->firstPart.Revision == ACPI_VERSION_1) | (!rsdp->XsdtAddress)) {
        RSDT *rsdt = (RSDT*)GetVirtualAddress(CachingModeWriteBack, (void*)(uint64_t)rsdp->firstPart.RsdtAddress);
        if (!ACPITables_ValidateChecksum((ACPISDTHeader*)rsdt)) return NULL;

        int entries = RSDT_GET_POINTER_COUNT((rsdt->h));
        int cur_index = 0;

        for (int i = 0; i < entries; i++) {
            ACPISDTHeader *h = (ACPISDTHeader*)GetVirtualAddress(CachingModeWriteBack, (void*)(uint64_t)rsdt->PointerToOtherSDT[i]);
            if (!strncmp(h->Signature, table_name, 4) && ACPITables_ValidateChecksum(h)) {
                if (cur_index == index)
                    return (void *) h;

                cur_index++;
            }
        }
    }

    return NULL;
}
