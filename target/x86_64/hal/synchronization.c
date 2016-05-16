#include "synchronization.h"
#include "CPUID/cpuid.h"
#include "kmalloc.h"

Spinlock
CreateSpinlock(void)
{
	return (Spinlock)kmalloc(CPUID_GetCacheLineSize());
}

bool
LockSpinlock(Spinlock *primitive)
{
    if(primitive == NULL)return FALSE;
	    __asm__ volatile
    (
        ".spin:"
        "\n\tpause"
        "\n\ttestl $1, (%0)"
        "\n\tjnz .spin"
        "\n\tlock btsl $0, (%0)"
        "\n\tjc .spin" :: "a"(primitive)
    );
    return TRUE;
}

bool
UnlockSpinlock(Spinlock *primitive)
{    
    if(primitive == NULL)return FALSE;
	__asm__ volatile
    (
        "movl $0, (%0)" :: "a"(primitive)
    );
    return TRUE;
}

void
FreeSpinlock(Spinlock *primitive)
{
    if(primitive != NULL)kfree(primitive);
}