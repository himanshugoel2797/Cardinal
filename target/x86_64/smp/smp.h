#ifndef _CARDINAL_X86_64_SMP_H_
#define _CARDINAL_X86_64_SMP_H_

void
SMP_IncrementCoreCount(void);

void
SMP_LockTrampoline(void);

void
SMP_UnlockTrampoline(void);

int
SMP_GetCoreCount(void);

void
SMP_WaitForCoreCountIncrement(void);

#endif
