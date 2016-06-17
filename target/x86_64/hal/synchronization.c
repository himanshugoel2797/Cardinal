#include "synchronization.h"
#include "CPUID/cpuid.h"
#include "managers.h"
#include "kmalloc.h"
#include "common.h"

Spinlock
CreateSpinlock(void) {
    Spinlock p = kmalloc(CPUID_GetCacheLineSize());
    memset(p, 0, CPUID_GetCacheLineSize());
    return p;
}

Spinlock
CreateBootstrapSpinlock(void) {
    Spinlock p = (Spinlock)bootstrap_malloc(CPUID_GetCacheLineSize());
    memset(p, 0, CPUID_GetCacheLineSize());
    return p;
}

bool
LockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;
    __asm__ volatile
    (
        ".acquire:\n\t"
        "cli\n\t"
        "lock btsl $0, (%0)\n\t"
        "jnc .acquired\n\t"
        ".spin:\n\t"
        "sti\n\t"
        "pause\n\t"
        "btl $0, (%0)\n\t"
        "jnc .spin\n\t"
        "cli\n\t"
        "lock btsl $0, (%0)\n\t"
        "jc .spin\n\t"
        ".acquired:\n\t"
        "sti" :: "a"(primitive)
    );
    return TRUE;
}

bool
UnlockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;
    __asm__ volatile
    (
        "movl $0, (%0)" :: "a"(primitive)
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL)kfree(primitive);
}