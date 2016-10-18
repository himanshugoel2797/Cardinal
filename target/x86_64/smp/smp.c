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
    while((curCC == SMP_GetCoreCount()) | smp_lock) __asm__ volatile("pause");

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
