#include "lpc.h"
#include "utils/native.h"

static uint16_t acpi_base_address;

uint32_t
LPC_Initialize(void) {
    int n = 0;
    for(n = 0; n <= pci_deviceCount; n++) {
        if(n == pci_deviceCount)return 0;
        if(pci_devices[n].classCode == 0x06 && pci_devices[n].subClassCode == 0x01) {
            break;
        }
    }

    acpi_base_address = (uint16_t)(pci_readDWord(
                                       pci_devices[n].bus,
                                       pci_devices[n].device,
                                       pci_devices[n].function,
                                       0x40) & ~1);

    AOS_Devices device;
    strcpy(device.name, "LPC Controller");
    device.Initialize = NULL;
    device.Disable = NULL;
    device.Enable = NULL;
    device.SetPowerState = LPC_TransitionPowerState;
    device.SaveState = NULL;
    device.RestoreState = NULL;
    device.deviceType = AOS_CHIPSET_DEVICE;
    device.devicePerms = AOS_DEVICE_PERMS_NONE;

    DeviceManager_RegisterDevice(&device);


    outl(acpi_base_address + 0x60, 0x80);
    outl(acpi_base_address + 0x61, 0x80);
    outl(acpi_base_address + 0x62, 0x80);
    outl(acpi_base_address + 0x63, 0x80);

    outl(acpi_base_address + 0x68, 0x80);
    outl(acpi_base_address + 0x69, 0x80);
    outl(acpi_base_address + 0x6A, 0x80);
    outl(acpi_base_address + 0x6B, 0x80);

    for(int i = 16; i < 24; i++) {
        IOAPIC_MapIRQ(i, IRQ(i), APIC_GetID(), 0, 0, 0);
        IOAPIC_SetEnableMode(IRQ(i), ENABLE);
    }

    return 1;
}

uint32_t
LPC_TransitionPowerState(AOS_PowerStates ps) {
    uint32_t addr = inl(acpi_base_address + 0x04);
    addr &= ~(15 << 10);
    switch(ps) {
    case AOS_D0:
        //The addr field has already been set to 0
        break;
    case AOS_D1:
        //Do nothing
        break;
    case AOS_D2:
        //Do nothing
        break;
    case AOS_D3:
        addr |= 1 << 10;	//Set to sleep mode
        break;
    case AOS_D4:
        addr |= 5 << 10;	//Set to suspent to RAM
        break;
    case AOS_D5:
        addr |= 7 << 10;	//Soft-Off
        break;
    }
    addr |= (1 << 13);
    outl(acpi_base_address + 0x04, addr);
}