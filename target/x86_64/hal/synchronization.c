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

    uint16_t dummy0 = 0;
    uint64_t dummy1 = 0;
    uint64_t dummy2 = 0;

    __asm__ volatile
    (
        "mfence\n\t"
        "pushfq\n\t"
        "cli\n\t"
        "popq %[rcx]\n\t"
        "shlq $16, %[rcx]\n\t"
        "movw $1, %[cx]\n\t"
        "lock xaddw %[cx], +2(%[prim])\n\t"
        "cmpw %[cx], (%[prim])\n\t"
        "je 3f\n\t"
        "1:\n\t"
        "btq $25, %[rcx]\n\t"
        "jnc 2f\n\t"
        "2:"
        "pause\n\t"
        "cmpw %[cx], (%[prim])\n\t"
        "jne 1b\n\t"
        "3:\n\t"
        "btq $25, %[rcx]\n\t"
        "jnc 4f\n\t"
        "orw $1, +4(%[prim])\n\t"
        "4:\n\t"
        "cli\n\t"
        :: [prim]"r"(primitive), [cx]"r"(dummy0), [rcx]"r"(dummy1), [rdx]"r"(dummy2) 
        : "memory"
    );
    return TRUE;
}

uint64_t
GetSpinlockContenderCount(Spinlock primitive) {
    uint64_t cnt = 0;
    __asm__ volatile
    (
        "movq (%[prim]), %[cnt]\n\t"
        : [cnt]"=r"(cnt) : [prim]"r"(primitive) :
    );

    cnt = ((cnt >> 16) & 0xFFFF) - (cnt & 0xFFFF);
    return cnt;
}

bool
UnlockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;
    __asm__ volatile
    (
        "mfence\n\t"
        "lock incw (%0)\n\t"
        "btw $0, +4(%0)\n\t"
        "jnc 1f\n\t"
        "sti\n\t"
        "movw $0, +4(%0)\n\t"
        "1:\n\t"
        :: "r"(primitive) : "memory"
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL)kfree(primitive);
}