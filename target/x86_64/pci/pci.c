#include "pci.h"
#include "drivers/drivers.h"
#include "acpi/acpi_tables.h"
#include "acpi/mcfg.h"
#include "managers.h"
#include "common.h"
#include "utils/native.h"
#include "apic/apic.h"

#define MAX_DEVICE_COUNT 128
#define PCI_MASS_STORAGE_DEVICE_CLASS 0x1
#define PCI_BUS_MASTER_CMD (1<<2)
#define PCI_ADDR 0xCF8
#define PCI_DATA 0xCFC


static MCFG_Entry *mcfg_table;
static uint64_t mcfg_entry_count;

static PCI_DeviceFuncs pci_devices[MAX_DEVICE_COUNT];
static uint32_t pci_deviceCount;

uint32_t
pci_getDeviceCount(void) {
    return pci_deviceCount;
}

void
pci_getDeviceInfo(uint32_t pci_index,
                  PCI_DeviceFuncs *p) {
    if(p == NULL | pci_index >= pci_deviceCount)return;
    memcpy(p, &pci_devices[pci_index], sizeof(PCI_DeviceFuncs));
}

uint32_t
pci_readDWord(
    uint32_t bus,
    uint32_t device,
    uint32_t function,
    uint32_t offset
) {
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    return inl(PCI_DATA);
}

void
pci_writeDWord(
    uint32_t bus,
    uint32_t device,
    uint32_t function,
    uint32_t offset,
    uint32_t val
) {
    outl(PCI_ADDR, 0x80000000 | bus << 16 | device << 11 | function <<  8 | (offset & 0xfc));
    outl(PCI_DATA, val);
}

bool
pci_enableMSI(uint32_t device_index) {
    bool msi_64b_cap = FALSE;

    uint32_t reg = pci_readDWord(
                       pci_devices[device_index].bus,
                       pci_devices[device_index].device,
                       pci_devices[device_index].function,
                       0x04);
    reg = reg & ~(1<<10);   //Disable legacy interrupts
    reg |= (1 << 2);        //Enable bus mastering for MSI
    pci_writeDWord(
        pci_devices[device_index].bus,
        pci_devices[device_index].device,
        pci_devices[device_index].function,
        0x04,
        reg);


    uint32_t cap_list_off = pci_readDWord(
                                pci_devices[device_index].bus,
                                pci_devices[device_index].device,
                                pci_devices[device_index].function,
                                0x34) & 0xFF;

    uint32_t msi_ctrl_off = cap_list_off << 8;
    uint32_t msi_ctrl_off_nx = msi_ctrl_off & 0xFFFF;

    while( (msi_ctrl_off_nx & 0xFF) != 0x05) {
        msi_ctrl_off = msi_ctrl_off_nx;
        if(msi_ctrl_off == 0)return FALSE;

        msi_ctrl_off_nx = pci_readDWord(
                              pci_devices[device_index].bus,
                              pci_devices[device_index].device,
                              pci_devices[device_index].function,
                              msi_ctrl_off >> 8) & 0xFFFF;
    }

    msi_ctrl_off = msi_ctrl_off >> 8;

    reg = pci_readDWord(
              pci_devices[device_index].bus,
              pci_devices[device_index].device,
              pci_devices[device_index].function,
              msi_ctrl_off);

    msi_64b_cap = (reg >> 16) >> 7;
    reg = (1 << 16);        //Enable MSI


    pci_writeDWord(
        pci_devices[device_index].bus,
        pci_devices[device_index].device,
        pci_devices[device_index].function,
        msi_ctrl_off,
        reg);


    reg = pci_readDWord(
              pci_devices[device_index].bus,
              pci_devices[device_index].device,
              pci_devices[device_index].function,
              msi_ctrl_off + 4);
    reg = 0xFEE00000;   //Set the MSI Address as per x86 specs
    reg |= APIC_GetID() << 12;  //Set the APIC address

    pci_writeDWord(
        pci_devices[device_index].bus,
        pci_devices[device_index].device,
        pci_devices[device_index].function,
        msi_ctrl_off + 4,
        reg);

    return TRUE;
}

void
pci_regMSIVector(uint32_t device_index,
                 uint8_t vector) {


    uint32_t cap_list_off = pci_readDWord(
                                pci_devices[device_index].bus,
                                pci_devices[device_index].device,
                                pci_devices[device_index].function,
                                0x34) & 0xFF;

    uint32_t msi_ctrl_off = cap_list_off << 8;
    uint32_t msi_ctrl_off_nx = msi_ctrl_off & 0xFFFF;

    while( (msi_ctrl_off_nx & 0xFF) != 0x05) {
        msi_ctrl_off = msi_ctrl_off_nx;

        msi_ctrl_off_nx = pci_readDWord(
                              pci_devices[device_index].bus,
                              pci_devices[device_index].device,
                              pci_devices[device_index].function,
                              msi_ctrl_off >> 8) & 0xFFFF;

    }

    msi_ctrl_off = msi_ctrl_off >> 8;


    uint32_t reg = pci_readDWord(
                       pci_devices[device_index].bus,
                       pci_devices[device_index].device,
                       pci_devices[device_index].function,
                       msi_ctrl_off);
    bool msi_64b_cap = (reg >> 16) >> 7;
    pci_writeDWord(
        pci_devices[device_index].bus,
        pci_devices[device_index].device,
        pci_devices[device_index].function,
        msi_ctrl_off,
        reg);


    uint32_t data_reg_off = msi_ctrl_off+ 8;
    if(msi_64b_cap)data_reg_off += 4;

    reg = pci_readDWord(
              pci_devices[device_index].bus,
              pci_devices[device_index].device,
              pci_devices[device_index].function,
              data_reg_off);

    reg &= ~0xFF;
    reg |= vector;   //Set the MSI vector as per x86 specs

    pci_writeDWord(
        pci_devices[device_index].bus,
        pci_devices[device_index].device,
        pci_devices[device_index].function,
        data_reg_off,
        reg);
}

void
pci_Initialize(void) {
    MCFG *mcfg = ACPITables_FindTable(MCFG_SIG, 0);

    mcfg_entry_count = (mcfg->h.Length - sizeof(ACPISDTHeader) - 8)/sizeof(MCFG_Entry);
    mcfg_table = bootstrap_malloc(mcfg_entry_count * sizeof(MCFG_Entry));
    memcpy(mcfg_table, mcfg->entries, sizeof(MCFG_Entry) * mcfg_entry_count);

    //Change addresses to virtual addresses, detect all devices and register them to the OS
    //The OS may then eventually initialize the relevant drivers
    //Enumerate PCI pci_devices
    for(int bus = 0; bus < 256; bus++)
        for(int device = 0; device < 32; device++) {
            uint32_t vid = pci_readDWord(bus, device, 0, 0);
            if( (vid >> 16) != 0xFFFF) {
                int headerType = pci_readDWord(bus, device, 0, 0x0C);
                int functionCount = 1;
                if( (headerType >> 23) & 1) functionCount = 8;

                for(int f = 0; f < functionCount; f++)
                    if(pci_readDWord(bus, device, f, 0) >> 16 != 0xFFFF) {

                        pci_devices[pci_deviceCount].classCode = pci_readDWord(bus, device, f, 8) >> 24;
                        pci_devices[pci_deviceCount].subClassCode = pci_readDWord(bus, device, f, 8) >> 16;
                        pci_devices[pci_deviceCount].progIf = pci_readDWord(bus, device, f, 8) >> 8;
                        pci_devices[pci_deviceCount].bus = bus;
                        pci_devices[pci_deviceCount].device = device;
                        pci_devices[pci_deviceCount].function = f;
                        pci_devices[pci_deviceCount].headerType = (pci_readDWord(bus, device, f, 0x0C) >> 16) & 0xFF;

                        pci_devices[pci_deviceCount].deviceID = pci_readDWord(bus, device, f, 0) >> 16;
                        pci_devices[pci_deviceCount].vendorID = pci_readDWord(bus, device, f, 0);

                        switch(pci_devices[pci_deviceCount].headerType) {
                        case 0:
                            pci_devices[pci_deviceCount].bar_count = 6;
                            break;
                        case 1:
                            pci_devices[pci_deviceCount].bar_count = 2;
                            break;
                        }

                        uint32_t bar_index_v = 0;
                        uint32_t bar_cnt = pci_devices[pci_deviceCount].bar_count;

                        for(uint8_t bar_index = 0; bar_index < bar_cnt; bar_index++) {
                            uint64_t bar_val = pci_readDWord(bus, device, f, 0x10 + (bar_index * 4));

                            pci_devices[pci_deviceCount].bars[bar_index].offset = 0x10 + (bar_index * 4);
                            pci_devices[pci_deviceCount].bars[bar_index].isIOSpace = (bar_val & 1);

                            if((bar_val & 0x3) == 0x2) {
                                bar_index++;
                                uint64_t bar_val_2 = pci_readDWord(bus, device, f, 0x10 + (bar_index * 4));
                                bar_val |= (bar_val_2 << 32);
                                pci_devices[pci_deviceCount].bar_count--;
                            }

                            if(bar_val & 1) {
                                pci_devices[pci_deviceCount].bars[bar_index].value = bar_val & 0xFFFFFFFFFFFFFFFC;
                            } else {
                                pci_devices[pci_deviceCount].bars[bar_index].value = bar_val & 0xFFFFFFFFFFFFFFF0;
                            }

                            bar_index_v++;
                        }


                        pci_deviceCount++;
                    }
            }
        }

    //Register the bus
    RegisterBus("PCI");

    //Go through all the built in drivers, load any that match the present hardware
    for(uint32_t i = 0; i < pci_deviceCount; i++) {
        int j = -1;
        while(drivers[++j].detector != NULL) {
            PCI_DeviceFuncs f;
            pci_getDeviceInfo(i, &f);
            if(drivers[j].detector(&f)) { //We provide the driver with a copy here because it doesn't own the device yet
                RegisterDevice(drivers[j].name,
                               drivers[j].bus_name,
                               drivers[j].dev_type,
                               drivers[j].handler);

                drivers[j].init(&pci_devices[i]);    //The driver now owns the device so it may change things as needed
                break;
            }
        }
    }
}