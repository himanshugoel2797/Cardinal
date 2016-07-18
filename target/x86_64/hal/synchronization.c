#include "synchronization.h"
#include "CPUID/cpuid.h"
#include "apic/apic.h"
#include "managers.h"
#include "kmalloc.h"
#include "common.h"

Spinlock
CreateSpinlock(void) {
    Spinlock p = kmalloc(CPUID_GetCacheLineSize());
    memset((void*)p, 0, CPUID_GetCacheLineSize());
    return p;
}

Spinlock
CreateBootstrapSpinlock(void) {
    Spinlock p = (Spinlock)bootstrap_malloc(CPUID_GetCacheLineSize());
    memset((void*)p, 0, CPUID_GetCacheLineSize());
    return p;
}

bool
LockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;

    register uint16_t dummy0 = 0;
    register uint64_t dummy1 = 0;
    uint64_t dummy2 = 0;

    dummy2 = APIC_GetID() + 1;
    if(dummy2 == 0)
        dummy2 = -1;

    //Test to see if the lock is set on the same core, if so, let it through
    //Else obtain a ticket and spin waiting for your turn
    __asm__ volatile
    (
        "mfence\n\t"
        "pushfq\n\t"
        "cli\n\t"
        "popq %[rcx]\n\t"
        "cmpq %[rdx], +8(%[prim])\n\t"
        "je 4f\n\t"
        "shlq $16, %[rcx]\n\t"
        "movw $1, %[cx]\n\t"
        "lock xaddw %[cx], +2(%[prim])\n\t"
        "cmpw %[cx], (%[prim])\n\t"
        "je 3f\n\t"
        "1:\n\t"
        "pause\n\t"
        "cmpw %[cx], (%[prim])\n\t"
        "jne 1b\n\t"
        "3:\n\t"
        "btq $25, %[rcx]\n\t"
        "jnc 4f\n\t"
        "movw $1, +4(%[prim])\n\t"
        "4:\n\t"
        "lock incw +6(%[prim])\n\t"
        "movq %[rdx], +8(%[prim])"
        :: [prim]"r"(primitive), [cx]"r"(dummy0), [rcx]"r"(dummy1), [rdx]"r"(dummy2)
        : "memory", "cc"
    );

    //Store the core number in the spinlock state in order to allow the lock to pass if the core number matches

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

    register uint16_t dummy0 = 0;

    __asm__ volatile
    (
        "mfence\n\t"
        "lock decw +6(%[prim])\n\t"
        "jnz 1f\n\t"
        "movq $0, +8(%[prim])\n\t"
        "xchgw %[dm0], +4(%[prim])\n\t"
        "lock incw (%[prim])\n\t"
        "btw $0, %[dm0]\n\t"
        "jnc 1f\n\t"
        "sti\n\t"
        "1:\n\t"
        :: [prim]"r"(primitive), [dm0]"r"(dummy0) : "memory", "cc"
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL)kfree((void*)primitive);
}

void
AtomicIncrement32(uint32_t *val) {
    __asm__ ("lock incl (%0)" :: "r"(val) : "memory", "cc");
}

void
AtomicIncrement64(uint64_t *val) {
    __asm__ ("lock incq (%0)" :: "r"(val) : "memory", "cc");
}

void
AtomicDecrement32(uint32_t *val) {
    __asm__ ("lock decl (%0)" :: "r"(val) : "memory", "cc");
}

void
AtomicDecrement64(uint64_t *val) {
    __asm__ ("lock decq (%0)" :: "r"(val) : "memory", "cc");
}