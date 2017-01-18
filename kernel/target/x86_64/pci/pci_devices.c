/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "pci_devices_names.h"
#include "types.h"

void
pci_GetPCIClass3 (
    unsigned char baseid,
    unsigned char subid,
    unsigned char progid,
    char **   basedesc,
    char **   subdesc,
    char **   progdesc
) {
    uint32_t i;

    *basedesc = *subdesc = *progdesc = "";

    for ( i=0; i < PCI_CLASSCODETABLE_LEN; ++i ) {
        if ( PciClassCodeTable[i].BaseClass == baseid ) {
            if ( !(**basedesc) )
                *basedesc = PciClassCodeTable[i].BaseDesc;
            if ( PciClassCodeTable[i].SubClass == subid ) {
                if ( !(**subdesc) )
                    *subdesc = PciClassCodeTable[i].SubDesc;
                if ( PciClassCodeTable[i].ProgIf == progid ) {
                    *progdesc = PciClassCodeTable[i].ProgDesc;
                    break;
                }
            }
        }
    }
}

void
pci_GetPCIClass (
    long classcode,
    char ** base,
    char ** sub,
    char ** prog
) {
    unsigned char baseid, subid, progid;

    baseid  = ((classcode >> 24) & 0xFF);
    subid  =  ((classcode >> 16) & 0xFF);
    progid  = ((classcode >>  8) & 0xFF);

    pci_GetPCIClass3 (baseid,subid,progid,base,sub,prog);
}

void
pci_GetPCIVendor(uint16_t venID,
                 char ** short_name,
                 char ** long_name) {
    uint32_t i;
    *short_name = *long_name = "Unknown";

    for(i = 0; i < PCI_VENTABLE_LEN; ++i) {
        if(PciVenTable[i].VenId == venID) {
            *short_name = PciVenTable[i].VenShort;
            *long_name = PciVenTable[i].VenFull;

            break;
        }
    }
}

void
pci_GetPCIDevice(uint16_t venID,
                 uint16_t devID,
                 char **chip_name,
                 char **chip_desc) {
    uint32_t i;
    *chip_name = *chip_desc = "Unknown";

    for(i = 0; i < PCI_DEVTABLE_LEN; ++i) {
        if(PciDevTable[i].VenId == venID && PciDevTable[i].DevId == devID) {
            *chip_name = PciDevTable[i].Chip;
            *chip_desc = PciDevTable[i].ChipDesc;

            break;
        }
    }
}