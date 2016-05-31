#include "syscall.h"
#include "utils/native.h"

__attribute__((naked, noreturn))
void
Syscall_Handler(void)
{

}

void
Syscall_Initialize(void)
{
	uint64_t star_val = (0x8ull >> 32) | (0x18ull >> 48);
	uint64_t lstar = (uint64_t)Syscall_Handler;
	uint64_t cstar = 0;
	uint64_t sfmask = 0;

	wrmsr(0xC0000081, star_val);
	wrmsr(0xC0000082, lstar);
	wrmsr(0xC0000083, cstar);
	wrmsr(0xC0000084, sfmask);
}