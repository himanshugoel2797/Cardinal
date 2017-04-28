/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "types.h"
#include "smp.h"
#include "synchronization.h"
#include "apic/apic.h"

volatile _Atomic int smp_coreCount = 0;
volatile int smp_lock = 0;

void
SMP_IncrementCoreCount(void) {
    smp_coreCount++;
    //AtomicIncrement32((uint32_t*)&smp_coreCount);
}

int
SMP_GetCoreCount(void) {
    return smp_coreCount;
}

void
SMP_WaitForCoreCount(int curCC) {
    //Make sure the trampoline has also been unlocked
    do {
        while(smp_lock) __asm__ volatile("pause");
    } while(SMP_GetCoreCount() == curCC);
}

void
SMP_LockTrampoline(void) {
    while(__sync_lock_test_and_set(&smp_lock, 1)) {
        while(smp_lock)__asm__ volatile("pause");
    }
}

void
SMP_UnlockTrampoline(void) {
    __sync_synchronize();
    smp_lock = 0;
}

void
SMP_SetupProcessor(void) {

    //Update cr0 to have the caches enabled.
    register uint64_t cr0;
    __asm__("movq %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(3 << 29);  //Enable caches
    cr0 |= (1 << 16);   //Set the Write Protect bit
    __asm__("movq %0, %%cr0" :: "r"(cr0));

}