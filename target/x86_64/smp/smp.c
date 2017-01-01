/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
