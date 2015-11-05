#include "types.h"
#include "memory.h"
#include "page_manager/phys_mem_manager.h"
#include "utils/native.h"
#include "common/common.h"
#include "virt_mem_manager.h"

#define PAT_MSR 0x277

#define MARK_PRESENT(a) (a = (a | 1))
#define MARK_WRITE(a) (a = (a | (1 << 1)))
#define MARK_USER(a) (a = (a | (1 << 2)))
#define MARK_NX(a) (a = (a | (1ull << 63)))
#define SET_ADDR(a, b) (a = (a | (b & ~0xfff)))

#define MARK_ABSENT(a) (a = (a & ~1))
#define MARK_READONLY(a) (a = (a & ~(1 << 1)))
#define MARK_KERNELONLY(a) (a = (a & ~(1 << 2)))
#define MARK_EXEC(a) (a = (a & ~(1ull << 63)))

#define SET_CACHEMODE(a, b) (a = (a | (b << 3)))

#define MARK_PSE(a) (a = (a | (1 << 7)))
#define MARK_GLOBAL_PSE(a) (a = (a | (1 << 8)))

#define GET_ADDR(a) (a & ~0xf000000000000fff)

static uint64_t* kernel_pdpt;
static PML_Instance curPML;

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

  	kernel_pdpt = (uint64_t*)pdpt_0;
  	pml[511] = (uint64_t)GetPhysicalAddress(pdpt_0) | 3;	//Keep the top 512GiB of memory mapped into all address spaces

  	memset (pdpt_0, 0, KiB(4));

  	uint64_t* pdpt_0_u64p = (uint64_t*)pdpt_0;
	pdpt_0_u64p[511] = 0x40000000 | 0x008B;
  	pdpt_0_u64p[510] = 0x00000000 | 0x008B;
  	pdpt_0_u64p[509] = 0xC0000000 | 0x008B;
  	pdpt_0_u64p[508] = 0x80000000 | 0x008B;

  	pdpt_0_u64p[507] = 0xC0000000 | 0x008B;
  	pdpt_0_u64p[506] = 0x80000000 | 0x008B;
  	pdpt_0_u64p[505] = 0x40000000 | 0x008B;
  	pdpt_0_u64p[504] = 0x00000000 | 0x008B;


  	pml = GetPhysicalAddress(pml);

  	__asm__ volatile("mov %%rax, %%cr3" :: "a"(pml));
}

PML_Instance
VirtMemMan_CreateInstance(void)
{
  PML_Instance pml = (void*)MemMan_Alloc();
  pml = (PML_Instance)GetVirtualAddress((void*)pml);
  memset ((void*)pml, 0, KiB(4));

  pml[511] = (uint64_t)GetPhysicalAddress(kernel_pdpt);
  MARK_PRESENT(pml[511]);
  MARK_WRITE(pml[511]);
  SET_CACHEMODE(pml[511], MEM_TYPE_WB);

  return pml;
}

PML_Instance
VirtMemMan_SetCurrent(PML_Instance instance)
{
  PML_Instance tmp = curPML;
  curPML = instance;
  __asm__ volatile("mov %%rax, %%cr3" :: "a"(instance));
  return tmp;
}

void
VirtMemMan_MapHPage(PML_Instance inst,
		    uint64_t virt_addr,
		    uint64_t phys_addr,
		    bool present,
		    MEM_TYPES cache,
		    MEM_ACCESS_PERMS access_perm,
		    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/GiB(1) * GiB(1);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  pdpt[pdpt_off] = phys_addr;
  if(present)MARK_PRESENT(pdpt[pdpt_off]);

  SET_CACHEMODE(pdpt[pdpt_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);
  if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
  if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}

void
VirtMemMan_MapLPage(PML_Instance inst,
		    uint64_t virt_addr,
		    uint64_t phys_addr,
		    bool present,
		    MEM_TYPES cache,
		    MEM_ACCESS_PERMS access_perm,
		    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/GiB(1) * GiB(1);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  pdpt[pdpt_off] = phys_addr;
  if(present)MARK_PRESENT(pdpt[pdpt_off]);

  SET_CACHEMODE(pdpt[pdpt_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);
  if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
  if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}

void
VirtMemMan_MapPage(PML_Instance inst,
		    uint64_t virt_addr,
		    uint64_t phys_addr,
		    bool present,
		    MEM_TYPES cache,
		    MEM_ACCESS_PERMS access_perm,
		    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/GiB(1) * GiB(1);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  pdpt[pdpt_off] = phys_addr;
  if(present)MARK_PRESENT(pdpt[pdpt_off]);

  SET_CACHEMODE(pdpt[pdpt_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);
  if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
  if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}
