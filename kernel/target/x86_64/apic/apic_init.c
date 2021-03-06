/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "x86_64_common.h"
#include "apic.h"
#include "io_apic/io_apic.h"
#include "acpi/acpi_tables.h"
#include "priv_apic.h"
#include "acpi/madt.h"
#include "memory.h"
#include "cpuid/cpuid.h"
#include "smp/smp.h"
#include "managers.h"
#include "pit/pit.h"

static APIC_SMPThreads *smp_threads;

uint32_t
APIC_Initialize(void) {
    //Initialize the local APIC
    CPUID_RequestInfo(CPUID_EAX_FIRST_PAGE, CPUID_ECX_IGNORE);
    uint8_t apic_available = CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_APIC);

    if(!apic_available) return -1;

    smp_threads = NULL;

    //Initialize the local APIC
    APIC_LockPIC();
    APIC_LocalInitialize();
    //Ask ACPI for the MADT table
    MADT *madt = ACPITables_FindTable(MADT_SIG, 0);


    PIT_Initialize();

    if(madt != NULL) {
        uint32_t len = madt->h.Length - 8 - sizeof(ACPISDTHeader);
        int passNum = 0;

        for(; passNum < 3; passNum++) {
            for(uint32_t i = 0; i < len; ) {
                MADT_EntryHeader *hdr = (MADT_EntryHeader*)&madt->entries[i];

                switch(hdr->type) {
                case MADT_LAPIC_ENTRY_TYPE: {
                    MADT_EntryLAPIC *lapic = (MADT_EntryLAPIC*)hdr;
                    if(passNum == 0) {
                        APIC_SMPThreads *thread = bootstrap_malloc(sizeof(APIC_SMPThreads));
                        thread->apic_id = lapic->apic_id;
                        thread->proc_id = lapic->processor_id;

                        //Insert this thread in front of the list
                        thread->next = smp_threads;
                        smp_threads = thread;
                    } else if(lapic->apic_id != APIC_GetID() && passNum == 2) {
                        APIC_SendIPI(lapic->apic_id, APIC_DESTINATION_SHORT_NONE, 0, 5);
                        PIT_Sleep(10);
                        int curCC = SMP_GetCoreCount();
                        APIC_SendIPI(lapic->apic_id, APIC_DESTINATION_SHORT_NONE, 0x0f, 6);
                        SMP_WaitForCoreCount(curCC);
                    }
                }
                break;
                case MADT_IOAPIC_ENTRY_TYPE: {
                    if(passNum != 0) break;
                    MADT_EntryIOAPIC *ioapic = (MADT_EntryIOAPIC*)hdr;

                    IOAPIC_Initialize((uint64_t)GetVirtualAddress(CachingModeUncachable,(void*)(uint64_t)ioapic->io_apic_base_addr), ioapic->global_sys_int_base);

                    for(int j = 0; j < 16; j++) {
                        IOAPIC_MapIRQ(j, j + 32, APIC_GetID(), 0, 0, APIC_DELIVERY_MODE_FIXED);
                    }
                }
                break;
                case MADT_ISAOVER_ENTRY_TYPE: {
                    if(passNum != 1) break;
                    MADT_EntryISAOVR *isaovr = (MADT_EntryISAOVR*)hdr;

                    int polarity = isaovr->flags & 3;
                    int triggerMode = (isaovr->flags >> 2) & 3;


                    if(isaovr->irq_src == 0) {

                        IOAPIC_MapIRQ(isaovr->global_sys_int,
                                      isaovr->irq_src + 32,
                                      APIC_GetID(),
                                      triggerMode >> 1,
                                      polarity >> 1,
                                      APIC_DELIVERY_MODE_FIXED);
                    } else {

                        IOAPIC_MapIRQ(isaovr->global_sys_int,
                                      isaovr->irq_src,
                                      APIC_GetID(),
                                      triggerMode >> 1,
                                      polarity >> 1,
                                      APIC_DELIVERY_MODE_FIXED);
                    }
                }
                break;
                case MADT_IOAPIC_NMI_ENTRY_TYPE: {

                }
                break;
                }
                i += hdr->entry_size;
                if(hdr->entry_size == 0) i += 8;

            }
        }

        __asm__ volatile("sti");    //At this point we're ready to handle interrupts
        APIC_CallibrateTimer();

    } else {
        PANIC("Failed to find MADT. Cannot continue.");
        return -1;
    }

    return 0;
}
