#include "synchronization.h"
#include "CPUID/cpuid.h"
#include "apic/apic.h"
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

    dummy2 = dummy1 = APIC_GetID() + 1;
    if(dummy1 == 0)
        dummy1 = dummy2 = -1;

    //Test to see if the lock is set on the same core, if so, let it through
    //Else obtain a ticket and spin waiting for your turn
    __asm__ volatile
    (
        "mfence\n\t"
        "cmpq %[rcx], +8(%[prim])\n\t"
        "je 4f\n\t"
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
        "pause\n\t"
        "cmpw %[cx], (%[prim])\n\t"
        "jne 1b\n\t"
        "3:\n\t"
        "btq $25, %[rcx]\n\t"
        "jnc 4f\n\t"
        "orw $1, +4(%[prim])\n\t"
        "4:\n\t"
        "lock incw +6(%[prim])\n\t"
        "movq %[rdx], +8(%[prim])"
        :: [prim]"r"(primitive), [cx]"r"(dummy0), [rcx]"r"(dummy1), [rdx]"r"(dummy2)
        : "memory"
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

    uint64_t id = APIC_GetID() + 1;
    if(id == 0)id = -1;

    __asm__ volatile
    (
        "mfence\n\t"
        "lock decw +6(%[prim])\n\t"
        "jnz 1f\n\t"
        "movq $0, +8(%[prim])\n\t"
        "lock incw (%[prim])\n\t"
        "jmp 2f\n\t"
        "2:\n\t"
        "btw $0, +4(%[prim])\n\t"
        "jnc 1f\n\t"
        "sti\n\t"
        "movw $0, +4(%[prim])\n\t"
        "1:\n\t"
        :: [prim]"r"(primitive), [id]"r"(id) : "memory"
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL)kfree(primitive);
}

void
AtomicIncrement32(uint32_t *val) {
    __asm__ ("lock incl %0" : "=m"(val) : "m"(val) : "memory");
}

void
AtomicIncrement64(uint64_t *val) {
    __asm__ ("lock incq %0" : "=m"(val) : "m"(val) : "memory");
}

void
AtomicDecrement32(uint32_t *val) {
    __asm__ ("lock decl %0" : "=m"(val) : "m"(val) : "memory");
}

void
AtomicDecrement64(uint64_t *val) {
    __asm__ ("lock decq %0" : "=m"(val) : "m"(val) : "memory");
}