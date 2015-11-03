#include "apic.h"
#include "io_apic/io_apic.h"

#include "acpi_tables/acpi_tables.h"
#include "acpi_tables/madt.h"

#include "drivers.h"
#include "cpuid.h"

uint32_t APIC_Initialize()
{
    //Initialize the local APIC
    uint8_t apic_available = CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_APIC);
    if(!apic_available) return -1;

    //Initialize the local APIC
    APIC_LocalInitialize();

    //Ask ACPI for the MADT table
    MADT *madt = ACPITables_FindTable(MADT_SIG, 0);

    int q = 1;

    if(madt != NULL)
        {

            while(madt != NULL)
                {

                    uint32_t len = madt->h.Length - 8 - sizeof(ACPISDTHeader);
                    int passNum = 0;

                    for(; passNum < 2; passNum++)
                        for(uint32_t i = 0; i < len; )
                            {
                                MADT_EntryHeader *hdr = (MADT_EntryHeader*)&madt->entries[i];

                                switch(hdr->type)
                                    {
                                    case MADT_LAPIC_ENTRY_TYPE:
                                    {
                                        if(passNum != 0) break;
                                        MADT_EntryLAPIC *lapic = hdr;
                                        //We don't need to do anything with these yet
                                        COM_WriteStr("\r\nLAPIC\r\n");
                                        COM_WriteStr("\tLen: %d\r\n", lapic->h.entry_size);
                                        COM_WriteStr("\tID: %u\r\n", lapic->apic_id);
                                        COM_WriteStr("\tProcessor ID: %x\r\n", lapic->processor_id);
                                        COM_WriteStr("\tFlags %d", lapic->flags);
                                    }
                                    break;
                                    case MADT_IOAPIC_ENTRY_TYPE:
                                    {
                                        if(passNum != 0) break;
                                        MADT_EntryIOAPIC *ioapic = hdr;

                                        COM_WriteStr("\r\nIOAPIC\r\n");
                                        COM_WriteStr("\tLen: %d\r\n", ioapic->h.entry_size);
                                        COM_WriteStr("\tID: %x\r\n", ioapic->io_apic_id);
                                        COM_WriteStr("\tBase Address: %x\r\n", ioapic->io_apic_base_addr);
                                        COM_WriteStr("\tGlobal Interrupt Base: %x\r\n", ioapic->global_sys_int_base);

                                        IOAPIC_Initialize(ioapic->io_apic_base_addr, ioapic->global_sys_int_base);

                                        for(int j = 0; j < 16; j++)
                                            {
                                                IOAPIC_MapIRQ(j, j + 32, APIC_GetID(), 0, 0, APIC_DELIVERY_MODE_FIXED);
                                            }
                                    }
                                    break;
                                    case MADT_ISAOVER_ENTRY_TYPE:
                                    {
                                        if(passNum == 0) break;
                                        MADT_EntryISAOVR *isaovr = hdr;

                                        int polarity = isaovr->flags & 3;
                                        int triggerMode = (isaovr->flags >> 2) & 3;

                                        if(isaovr->irq_src == 0)
                                            {

                                                IOAPIC_MapIRQ(isaovr->global_sys_int, isaovr->irq_src + 32, APIC_GetID(), triggerMode >> 1,polarity >> 1, APIC_DELIVERY_MODE_FIXED);
                                            }
                                        else
                                            {

                                                IOAPIC_MapIRQ(isaovr->global_sys_int, isaovr->irq_src, APIC_GetID(), triggerMode >> 1,polarity >> 1, APIC_DELIVERY_MODE_FIXED);
                                            }

                                        COM_WriteStr("\r\nISAOVR\r\n");
                                        COM_WriteStr("\tLen: %d\r\n", isaovr->h.entry_size);
                                        COM_WriteStr("\tIRQ: %d\r\n", isaovr->irq_src);
                                        COM_WriteStr("\tBus: %d\r\n", isaovr->bus_src);
                                        COM_WriteStr("\tGlobal Interrupt: %d\r\n", isaovr->global_sys_int);
                                    }
                                    break;
                                    case MADT_IOAPIC_NMI_ENTRY_TYPE:
                                    {
                                        if(passNum == 0)break;
                                        MADT_EntryIOAPIC_NMI *nmi = hdr;


                                    }
                                    break;
                                    default:
                                        if(passNum != 0) break;
                                        COM_WriteStr("UNKN\r\n");
                                        break;
                                    }
                                i += hdr->entry_size;
                                if(hdr->entry_size == 0) i += 8;
                            }
                    madt = ACPITables_FindTable(MADT_SIG, q++);
                }
        }
    else
        {
            COM_WriteStr("MADT not found!\r\n");
            return -1;
        }

    return 0;
}
