#include "types.h"
#include "memory.h"
#include "page_manager/phys_mem_manager.h"
#include "utils/native.h"
#include "common/common.h"
#include "virt_mem_manager.h"

#define PAT_MSR 0x277

void
VirtMemMan_Initialize(void)
{
  	//Setup the PAT stuff
	uint64_t pat = 0;
    	pat |= 0x6;                   //PAT0 WB
    	pat |= ((uint64_t)0x4) << 8;  //PAT1 WT
    	pat |= ((uint64_t)0x0) << 16; //PAT2 UC
    	pat |= ((uint64_t)0x1) << 24;  //PAT3
    	wrmsr(PAT_MSR, pat);

	//Allocate a new PML4 and set of pages
	PML_Instance pml = (void*)MemMan_Alloc ();
  	pml = (PML_Instance)GetVirtualAddress((void*)pml);

  	memset ((void*)pml, 0, KiB(4));

  	void* pdpt_0 = (void*)MemMan_Alloc ();
  	pdpt_0 = GetVirtualAddress (pdpt_0);
  	pml[511] = (uint64_t)GetPhysicalAddress(pdpt_0) | 3;

  	memset (pdpt_0, 0, KiB(4));

  	uint64_t* pdpt_0_u64p = (uint64_t*)pdpt_0;
	pdpt_0_u64p[511] = 0x40000000 | 0x108B;
  	pdpt_0_u64p[510] = 0x00000000 | 0x108B;
  	pdpt_0_u64p[509] = 0xC0000000 | 0x108B;
  	pdpt_0_u64p[508] = 0x80000000 | 0x108B;

  	pdpt_0_u64p[507] = 0xC0000000 | 0x108B;
  	pdpt_0_u64p[506] = 0x80000000 | 0x108B;
  	pdpt_0_u64p[505] = 0x40000000 | 0x108B;
  	pdpt_0_u64p[504] = 0x00000000 | 0x108B;


  	pml = GetPhysicalAddress(pml);

  	__asm__ volatile("mov %%rax, %%cr3" :: "a"(pml));
}


