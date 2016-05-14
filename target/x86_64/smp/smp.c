#include "types.h"
#include "smp.h"
#include "apic/apic.h"

static int smp_coreCount = 0;
static int smp_lock = 0;

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
    int curCC = SMP_GetCoreCount();
    while(curCC == SMP_GetCoreCount());
}

void
SMP_LockTrampoline(void) {
    __asm__ volatile
    (
        ".spin:"
        "\n\tpause"
        "\n\ttestl $1, (%%rax)"
        "\n\tjnz .spin"
        "\n\tlock btsl $0, (%%rax)"
        "\n\tjc .spin" :: "a"(&smp_lock)
    );
}

void
SMP_UnlockTrampoline(void) {
    __asm__ volatile
    (
        "movl $0, (%%rax)" :: "a"(&smp_lock)
    );
}
