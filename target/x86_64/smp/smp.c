#include "types.h"
#include "smp.h"
#include "apic/apic.h"

static volatile int smp_coreCount = 0;
static volatile int smp_lock = 0;

void
SMP_IncrementCoreCount(void) {
    smp_coreCount++;
}

int
SMP_GetCoreCount(void) {
    return smp_coreCount;
}

void
SMP_WaitForCoreCountIncrement(void) {
    volatile int curCC = SMP_GetCoreCount();
    while(curCC == SMP_GetCoreCount());
}

void
SMP_LockTrampoline(void) {
    __asm__ volatile
    (
        "1:"
        "\n\tpause"
        "\n\ttestl $1, (%0)"
        "\n\tjnz 1b"
        "\n\tlock btsl $0, (%0)"
        "\n\tjc 1b" :: "r"(&smp_lock)
    );
}

void
SMP_UnlockTrampoline(void) {
    __asm__ volatile
    (
        "movl $0, (%0)" :: "r"(&smp_lock)
    );
}
