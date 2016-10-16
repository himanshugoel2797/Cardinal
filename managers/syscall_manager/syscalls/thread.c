#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
set_tid_address(void* address) {
    SetClearChildTIDAddress(GetCurrentThreadUID(),
                            address);

    return GetCurrentThreadUID();
}