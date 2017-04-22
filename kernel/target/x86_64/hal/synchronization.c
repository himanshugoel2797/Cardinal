/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "synchronization.h"
#include "CPUID/cpuid.h"
#include "apic/apic.h"
#include "managers.h"
#include "kmalloc.h"
#include "common.h"

#include "utils/native.h"

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
IntLockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;

#ifdef _TICKETED_SPINLOCK_
    register uint16_t dummy0 = 0;
    register uint64_t dummy1 = 0;

    //Test to see if the lock is set on the same core, if so, let it through
    //Else obtain a ticket and spin waiting for your turn
    __asm__ volatile
    (
        "mfence\n\t"
        "pushfq\n\t"
        "cli\n\t"
        "popq %[rcx]\n\t"
        "shlq $16, %[rcx]\n\t"
        "movw $1, %[cx]\n\t"
        "lock xaddw %[cx], +24(%[prim])\n\t"
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
        :: [prim]"r"(primitive), [cx]"r"(dummy0), [rcx]"r"(dummy1)
        : "memory", "cc"
    );

    //Store the core number in the spinlock state in order to allow the lock to pass if the core number matches
#elif _ASM_SPINLOCK_
    register uint64_t dummy1 = (uint64_t)__builtin_return_address(2);

    __asm__ volatile
    (
        "mfence\n\t"
        //Replace with attempt to acquire lock here
        "lock bts $0, (%[prim])\n\t"
        "jnc 3f\n\t"
        "1:\n\t"
        "pause\n\t"
        //Replace with attempt to acquire lock here
        "testw $1, (%[prim])\n\t"
        "jnz 1b\n\t"
        "lock bts $0, (%[prim])\n\t"
        "jc 1b\n\t"
        "3:\n\t"
        :: [prim]"r"(primitive), [rcx]"a"(dummy1)
        : "memory", "cc"
    );
#else

    volatile uint64_t *prim = (volatile uint64_t *)primitive;
    while(__sync_lock_test_and_set(prim, 1)) {
        while(*prim)__asm__ volatile("pause");
    }

#endif

    return TRUE;
}

bool
IntUnlockSpinlock(Spinlock primitive) {
    if(primitive == NULL)return FALSE;

#ifdef _TICKETED_SPINLOCK_
    register uint16_t dummy0 = 0;

    __asm__ volatile
    (
        "mfence\n\t"
        "lock incw (%[prim])\n\t"
        :: [prim]"r"(primitive), [dm0]"r"(dummy0) : "memory", "cc"
    );
    return TRUE;

#elif _ASM_SPINLOCK_
    register uint16_t dummy0 = 0;

    __asm__ volatile
    (
        "mfence\n\t"
        //Replace with attempt to free lock here
        "movw $0, (%[prim])\n\t"
        :: [prim]"r"(primitive), [dm0]"r"(dummy0) : "memory", "cc"
    );

    return TRUE;
#else
    volatile uint64_t *prim = (volatile uint64_t *)primitive;
    __sync_synchronize();
    *prim = 0;
    return TRUE;
#endif
}

uint64_t
GetSpinlockContenderCount(Spinlock primitive) {

#ifdef _TICKETED_SPINLOCK_
    uint64_t cnt = 0;
    __asm__ volatile
    (
        "movq (%[prim]), %[cnt]\n\t"
        : [cnt]"=r"(cnt) : [prim]"r"(primitive) :
    );

    cnt = ((cnt >> 16) & 0xFFFF) - (cnt & 0xFFFF);
    return cnt;
#else

    int cnt = 0;

    IntLockSpinlock(primitive);
    volatile uint64_t *prim = (volatile uint64_t*)primitive;
    cnt = prim[2];
    IntUnlockSpinlock(primitive);

    return cnt;
#endif
}

bool
TryLockSpinlock(Spinlock primitive) {

    bool locked = FALSE;
    uint64_t iflag = 0;
    __asm__ volatile("pushfq\n\tcli\n\tpopq %0" : "=r"(iflag) :: "cc");

    IntLockSpinlock(primitive);
    volatile uint64_t *prim = (volatile uint64_t*)primitive;
    
    if(prim[1] == (APIC_GetID() + 1)) {
        locked = TRUE;
        prim[2]++;
    } else if(prim[4] == 1) {
        locked = FALSE;
    } else if(prim[1] == 0 && prim[2] == 0) {
        prim[2] = 1;
        prim[1] = APIC_GetID() + 1;
        locked = TRUE;
        prim[3] = iflag;
    }

    __asm__ volatile("":::"memory");
    IntUnlockSpinlock(primitive);

    if(!locked)
        __asm__ volatile("push %0\n\tpopfq" :: "r"(iflag) : "cc");

    return locked;
}

Spinlock
LockSpinlock(Spinlock primitive) {

    uint64_t deleting = FALSE;
    bool locked = FALSE;

    while(!locked) {
        uint64_t iflag = 0;
        __asm__ volatile("pushfq\n\tcli\n\tpopq %0" : "=r"(iflag) :: "cc");

        IntLockSpinlock(primitive);
        volatile uint64_t *prim = (volatile uint64_t*)primitive;
        if(prim[1] == (APIC_GetID() + 1)) {
            locked = TRUE;
            prim[2]++;
        } else if(prim[4] == 1) { 
            locked = FALSE;
            deleting = TRUE;
        } else if(prim[1] == 0 && prim[2] == 0) {
            prim[2] = 1;
            prim[1] = APIC_GetID() + 1;
            locked = TRUE;
            prim[3] = iflag;
        }

        __asm__ volatile("":::"memory");
        IntUnlockSpinlock(primitive);

        if(!locked) {
            __asm__ volatile("push %0\n\tpopfq" :: "r"(iflag) : "cc");
            __asm__("pause");
        }

        if(deleting)
            return NULL;
    }
    return primitive;
}

void
AutounlockSpinlock(Spinlock *prim) {
    UnlockSpinlock(*prim);
}

bool
UnlockSpinlock(Spinlock primitive) {
    //outb(0x3f8, 'U');
    bool locked = FALSE;
    uint64_t iflag = 0;

    IntLockSpinlock(primitive);
    volatile uint64_t *prim = (volatile uint64_t*)primitive;
    if(prim[1] == (APIC_GetID() + 1)) {
        locked = TRUE;
        prim[2]--;
        if(prim[2] == 0) {
            prim[1] = 0;
            iflag = prim[3];
        }
    }
    __asm__ volatile("":::"memory");
    IntUnlockSpinlock(primitive);

    if(iflag != 0)
        __asm__ volatile("pushq %0\n\tpopfq" :: "r"(iflag) : "cc");

    return locked;
}

Spinlock
FinalLockSpinlock(Spinlock primitive) {

    bool deleting = FALSE;
    uint64_t iflag = 0;
    __asm__ volatile("pushfq\n\tcli\n\tpopq %0" : "=r"(iflag) :: "cc");
    IntLockSpinlock(primitive);
    volatile uint64_t *prim = (volatile uint64_t*)primitive;

    if(prim[4] == 1)
        deleting = TRUE;    //Another thread has already started the deletion, abort.
    else
        prim[4] = 1;    //Prevent any further locks from succeeding, anything that uses this lock should now fail.
    
    __asm__ volatile("":::"memory");
    IntUnlockSpinlock(primitive);
    __asm__ volatile("push %0\n\tpopfq" :: "r"(iflag) : "cc");

    if(deleting)
        return NULL;

    //Now everything else will stop trying to obtain a lock, allowing the deletion process to safely lock.
    bool locked = FALSE;

    while(!locked) {
        iflag = 0;
        __asm__ volatile("pushfq\n\tcli\n\tpopq %0" : "=r"(iflag) :: "cc");

        IntLockSpinlock(primitive);
        volatile uint64_t *prim = (volatile uint64_t*)primitive;
        if(prim[1] == (APIC_GetID() + 1)) {
            locked = TRUE;
            prim[2]++;
        } else if(prim[1] == 0 && prim[2] == 0) {
            prim[2] = 1;
            prim[1] = APIC_GetID() + 1;
            locked = TRUE;
            prim[3] = iflag;
        }

        __asm__ volatile("":::"memory");
        IntUnlockSpinlock(primitive);

        if(!locked) {
            __asm__ volatile("push %0\n\tpopfq" :: "r"(iflag) : "cc");
            __asm__("pause");
        }
    }

    return primitive;

}


void
FreeSpinlock(Spinlock primitive) {
    if(primitive != NULL) {
        memset((void*)primitive, 0, CPUID_GetCacheLineSize());
        kfree((void*)primitive);
    }
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
AtomicIncrementSize(size_t *val) {
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

void
AtomicDecrementSize(size_t *val) {
    __asm__ ("lock decq (%0)" :: "r"(val) : "memory", "cc");
}

void
AtomicSubtractSize(size_t *val, size_t sub) {
    __asm__ ("xadd %[sub], (%[val])" :: [val]"r"(val), [sub]"r"(-sub) : "memory", "cc");
}

void
AtomicAddSize(size_t *val, size_t add) {
    __asm__ ("xadd %[add], (%[val])" :: [val]"r"(val), [add]"r"(add) : "memory", "cc");
}