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
static PML_Instance curPML = (uint64_t*)0x1000;	//This is where the initial PML was placed;

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

  	//Setup kernel code map
	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFFC0000000,
			    0x40000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFF80000000,
			    0x00000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFF40000000,
			    0xC0000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFF00000000,
			    0x80000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	//Setup Lower 4GiB Identity Map
	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFEC0000000,
			    0xC0000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFE80000000,
			    0x80000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFE40000000,
			    0x40000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_MapHPage(pml,
			    0xFFFFFFFE00000000,
			    0x00000000,
			    TRUE,
			    MEM_TYPE_WB,
			    MEM_READ | MEM_WRITE | MEM_EXEC,
			    MEM_KERNEL);

	VirtMemMan_SetCurrent(pml);

  	//Enable the NX bit
	wrmsr(0xC0000080, rdmsr(0xC0000080) | (1 << 11));
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
  __asm__ volatile("mov %%rax, %%cr3" :: "a"(GetPhysicalAddress(instance)));
  return tmp;
}

PML_Instance
VirtMemMan_GetCurrent(void)
{
  return curPML;
}

static void
VirtMemMan_SetupPDPTEntry(PML_Instance inst,
                          uint32_t     pml_off)
{
  if(GET_ADDR(inst[pml_off]) == 0)
    {
      inst[pml_off] = MemMan_Alloc();
      MARK_PRESENT(inst[pml_off]);
      MARK_WRITE(inst[pml_off]);
      SET_CACHEMODE(inst[pml_off], MEM_TYPE_WB);
    }
}

static void
VirtMemMan_SetupPDEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off)
{
  VirtMemMan_SetupPDPTEntry(inst, pml_off);
  uint64_t *pdpt = (uint64_t*)GetVirtualAddress ((void*)GET_ADDR(inst[pml_off]));

  if(GET_ADDR(pdpt[pdpt_off]) == 0)
    {
      pdpt[pdpt_off] = MemMan_Alloc();
      MARK_PRESENT(pdpt[pdpt_off]);
      MARK_WRITE(pdpt[pdpt_off]);
      SET_CACHEMODE(pdpt[pdpt_off], MEM_TYPE_WB);
    }
}

static void
VirtMemMan_SetupPTEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off,
                        uint32_t     pd_off)
{
  VirtMemMan_SetupPDEntry(inst, pml_off, pdpt_off);

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress ((void*)GET_ADDR(inst[pml_off]));
  uint64_t *pd = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pdpt[pdpt_off]));

  if(GET_ADDR(pd[pd_off]) == 0)
    {
      pd[pd_off] = MemMan_Alloc();
      MARK_PRESENT(pd[pd_off]);
      MARK_WRITE(pd[pd_off]);
      SET_CACHEMODE(pd[pd_off], MEM_TYPE_WB);
    }
}

void
VirtMemMan_MapHPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/GiB(1) * GiB(1);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;

  VirtMemMan_SetupPDPTEntry(inst, pml_off);

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  pdpt[pdpt_off] = phys_addr;
  if(present)MARK_PRESENT(pdpt[pdpt_off]);

  MARK_PSE(pdpt[pdpt_off]);
  SET_CACHEMODE(pdpt[pdpt_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);

  if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
  else MARK_NX(pdpt[pdpt_off]);

  if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}

void
VirtMemMan_MapLPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/MiB(2) * MiB(2);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
  uint32_t pd_off = (virt_addr >> 21) & 0x1FF;

  VirtMemMan_SetupPDEntry(inst, pml_off, pd_off);

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  uint64_t *pd = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pdpt[pdpt_off]));

  pd[pd_off] = phys_addr;
  if(present)MARK_PRESENT(pd[pd_off]);

  MARK_PSE(pd[pd_off]);
  SET_CACHEMODE(pd[pd_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pd[pd_off]);

  if(access_perm & MEM_EXEC)MARK_EXEC(pd[pd_off]);
  else MARK_NX(pd[pd_off]);

  if(sec_perms & MEM_USER)MARK_USER(pd[pd_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}


void
VirtMemMan_MapSPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms)
{
  phys_addr = phys_addr/KiB(4) * KiB(4);	//Align the physical address

  uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
  uint32_t pd_off = (virt_addr >> 21) & 0x1FF;
  uint32_t pt_off = (virt_addr >> 12) & 0x1FF;

  VirtMemMan_SetupPTEntry(inst, pml_off, pd_off, pt_off);

  uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));
  uint64_t *pd = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pdpt[pdpt_off]));
  uint64_t *pt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pd[pd_off]));

  pt[pt_off] = phys_addr;
  if(present)MARK_PRESENT(pt[pt_off]);

  SET_CACHEMODE(pt[pt_off], cache);
  if(access_perm & MEM_WRITE)MARK_WRITE(pt[pt_off]);

  if(access_perm & MEM_EXEC)MARK_EXEC(pt[pt_off]);
  else MARK_NX(pt[pt_off]);

  if(sec_perms & MEM_USER)MARK_USER(pt[pt_off]);

  __asm__ volatile("invlpg (%%rax)" :: "a"(virt_addr));
}

/*void
VirtMemMan_Map(PML_Instance       inst,
               uint64_t           virt_addr,
               uint64_t           phys_addr,
               uint64_t           size,
               bool               present,
               MEM_TYPES          cache,
               MEM_ACCESS_PERMS   access_perm,
               MEM_SECURITY_PERMS sec_perms)
{
  //Determine the generally best mapping for the given size

  //Simple cases: both addresses are aligned to the page boundary, size is a page size
  //Complicated cases: addresses aren't aligned
  //		       size isn't aligned
}*/

void*
VirtMemMan_GetPhysicalAddress(PML_Instance  inst,
                              void         *addr)
{
  //Parse the paging structures to determine the physical address
  uint64_t p_addr = (uint64_t)addr;

  uint32_t pml_off = (p_addr >> 39) & 0x1FF;
  uint32_t pdpt_off = (p_addr >> 30) & 0x1FF;
  uint32_t pd_off = (p_addr >> 21) & 0x1FF;
  uint32_t pt_off = (p_addr >> 12) & 0x1FF;

  if(inst[pml_off] & 1)
    {
      uint64_t *pdpt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(inst[pml_off]));

      if(((pdpt[pdpt_off] >> 7) & 1) && (pdpt[pdpt_off] & 1))	//Check for PSE
	{
	  return (void*)(GET_ADDR(pdpt[pdpt_off]) + (p_addr % GiB(1)));
	}

      if(pdpt[pdpt_off] & 1)
	{
	  uint64_t *pd = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pdpt[pdpt_off]));

	  if(((pd[pd_off] >> 7) & 1) && (pd[pd_off] & 1))	//Check for PSE
	    {
	      return (void*)(GET_ADDR(pd[pd_off]) + (p_addr % MiB(2)));
	    }

	  uint64_t *pt = (uint64_t*)GetVirtualAddress((void*)GET_ADDR(pd[pd_off]));

	  if(pt[pt_off] & 1)
	    {
	      return (void*)(GET_ADDR(pt[pt_off]) + (p_addr % KiB(4)));
	    }
	}

    }

  return NULL;
}
