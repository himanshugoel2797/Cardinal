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
        "pushq %%rcx\n\t"
        "pushfq\n\t"
        "cli\n\t"
        "popq %%rcx\n\t"
        "shlq $16, %%rcx\n\t"
        "movw $1, %%cx\n\t"
        "lock xaddw %%cx, +2(%0)\n\t"
        "cmpw %%cx, (%0)\n\t"
        "je .acquired\n\t"
        ".spin:\n\t"
        "btq $25, %%rcx\n\t"
        "jnc .skip_sti\n\t"
        "sti\n\t"
        ".skip_sti:"
        "pause\n\t"
        "cmpw %%cx, (%0)\n\t"
        "jne .spin\n\t"
        ".acquired:\n\t"
        "btq $25, %%rcx\n\t"
        "jnc .skip_flag_store\n\t"
        "orw $1, +4(%0)\n\t"
        ".skip_flag_store:\n\t"
        "cli\n\t"
        "popq %%rcx\n\t"
        :: "a"(primitive)
    );
    return TRUE;
}

uint64_t
GetSpinlockContenderCount(Spinlock primitive)
{
    uint32_t cnt = 0;
    __asm__ volatile
    (
        "lock movl (%%rax), %%eax\n\t"
        : "=a"(cnt) : "a"(primitive) :
        );

    cnt = (cnt >> 16) - (cnt & 0xFFFF);
    return (uint64_t)cnt;
}

bool
UnlockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;
    __asm__ volatile
    (
        "lock incw (%0)\n\t"
        "btw $0, +4(%0)\n\t"
        "jnc .unlock_skip_sti\n\t" 
        "sti\n\t"
        "movw $0, +4(%0)\n\t"
        ".unlock_skip_sti:\n\t"
        :: "r"(primitive)
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL)kfree(primitive);
}