#include "managers.h"
#include "arch_syscalls.h"

#include "libs/libCardinal/include/syscall.h"

uint64_t
SecurityMonitor_IOPL(uint64_t val) {
    uint64_t rflag = GetRFLAGS();
    rflag = rflag & ~(3 << 12);

    SetRFLAGS(rflag | (val << 12));

    return 0;
}