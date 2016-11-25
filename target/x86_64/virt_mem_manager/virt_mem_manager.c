#include "thread.h"
#include "types.h"
#include "page_manager/phys_mem_manager.h"
#include "utils/native.h"
#include "common/common.h"
#include "virt_mem_manager.h"
#include "CPUID/cpuid.h"
#include "managers.h"
#include "IDT/idt.h"
#include "interrupts.h"
#include "memory.h"
#include "synchronization.h"

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

#define GET_WRITE(a) ((a & (1 << 1)))
#define GET_USER(a) ((a & (1 << 2)))
#define GET_NX(a)   ((a & (1ull << 63)))


#define GET_PRESENT(a) ((a & 1))
#define SET_CACHEMODE(a, b) (a = (a | (b << 3)))
#define GET_CACHEMODE(a) (((a >> 3) & 3))

#define MARK_PSE(a) (a = (a | (1 << 7)))
#define MARK_GLOBAL(a) (a = (a | (1 << 8)))
#define GET_PSE(a) (a & (1 << 7))
#define GET_GLOBAL(a) (a & (1 << 8))

#define GET_ADDR_4KB(a) (a & ~0xf000000000000fff)
#define GET_ADDR_2MB(a) (a & ~0xf0000000000fffff)
#define GET_ADDR_1GB(a) (a & ~0xf00000003fffffff)

#define CORE_LOCAL_MEM_ADDR (0xffffff0000000000)
#define BOOTSTRAP_PML_ADDR (0xfffffffe00001000)

static uint64_t* kernel_pdpt = NULL;
static uint64_t* kernel_pdpt_paddr = NULL;

typedef struct VirtMemManData {
    PML_Instance curPML;
    bool hugePageSupport;
    uint64_t coreLocalStorage;
} VirtMemManData;


static uint64_t coreLocalSpace;
static Spinlock vmem_lock = NULL;

static volatile VirtMemManData CORE_LOCAL* virtMemData = (volatile VirtMemManData CORE_LOCAL*)0;

void
VirtMemMan_InitializeBootstrap(void) {

    //Assigns to virtMemData
    SetGSBase(bootstrap_malloc(sizeof(VirtMemManData)));

    virtMemData->curPML = (uint64_t*)BOOTSTRAP_PML_ADDR;    //Where initial PML is located
    virtMemData->hugePageSupport = FALSE;

    if(vmem_lock == NULL) {
        vmem_lock = CreateBootstrapSpinlock();
    }
}

void
VirtMemMan_Initialize(void) {

    CPUID_RequestInfo(0x80000001, 0);
    virtMemData->hugePageSupport = CPUID_FeatureIsAvailable(CPUID_EDX, (1 << 26));

    //Setup the PAT stuff
    uint64_t pat = 0;
    pat |= 0x6;                   //PAT0 WB
    pat |= ((uint64_t)0x4) << 8;  //PAT1 WT
    pat |= ((uint64_t)0x0) << 16; //PAT2 UC
    pat |= ((uint64_t)0x1) << 24;  //PAT3 WC
    wrmsr(PAT_MSR, pat);

    //Allocate a new PML4 and set of pages
    PML_Instance pml = (void*)MemMan_Alloc ();
    pml = (PML_Instance)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pml);

    memset ((void*)pml, 0, KiB(4));


    if(kernel_pdpt == NULL) {
        void* pdpt_0 = (void*)MemMan_Alloc ();
        kernel_pdpt_paddr = (uint64_t*)pdpt_0;

        kernel_pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, pdpt_0);

        pml[511] = (uint64_t)pdpt_0;    //Keep the top 512GiB of memory mapped into all address spaces
        MARK_PRESENT(pml[511]);
        MARK_WRITE(pml[511]);
        SET_CACHEMODE(pml[511], MEM_TYPE_WB);
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

        //Setup Upper 4GiB map
        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFDC0000000,
                            0x1C0000000,
                            TRUE,
                            MEM_TYPE_WB,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFD80000000,
                            0x180000000,
                            TRUE,
                            MEM_TYPE_WB,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFD40000000,
                            0x140000000,
                            TRUE,
                            MEM_TYPE_WB,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFD00000000,
                            0x100000000,
                            TRUE,
                            MEM_TYPE_WB,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        //Setup lower 4GiB map
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

        //Uncached
        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFB00000000,
                            0x00000000,
                            TRUE,
                            MEM_TYPE_UC,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFB40000000,
                            0x40000000,
                            TRUE,
                            MEM_TYPE_UC,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFB80000000,
                            0x80000000,
                            TRUE,
                            MEM_TYPE_UC,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFBC0000000,
                            0xC0000000,
                            TRUE,
                            MEM_TYPE_UC,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        //Write Through
        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFC00000000,
                            0x00000000,
                            TRUE,
                            MEM_TYPE_WT,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFC40000000,
                            0x40000000,
                            TRUE,
                            MEM_TYPE_WT,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFC80000000,
                            0x80000000,
                            TRUE,
                            MEM_TYPE_WT,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        VirtMemMan_MapHPage(pml,
                            0xFFFFFFFCC0000000,
                            0xC0000000,
                            TRUE,
                            MEM_TYPE_WT,
                            MEM_READ | MEM_WRITE | MEM_EXEC,
                            MEM_KERNEL);

        coreLocalSpace = APLS_SIZE;
    } else {
        pml[511] = (uint64_t)kernel_pdpt_paddr;
        MARK_PRESENT(pml[511]);
        MARK_WRITE(pml[511]);
        SET_CACHEMODE(pml[511], MEM_TYPE_WB);
    }

    //Enable NX
    wrmsr(0xC0000080, rdmsr(0xC0000080) | (1 << 11));


    virtMemData->curPML = pml;
    virtMemData->coreLocalStorage = MemMan_Alloc4KiBPageCont(APLS_SIZE/PAGE_SIZE);
    VirtMemMan_SetCurrent(pml);
}

void*
VirtMemMan_AllocCoreLocalData(uint64_t size) {
    if(size <= coreLocalSpace && coreLocalSpace > 0) {
        uint64_t addr = CORE_LOCAL_MEM_ADDR + (APLS_SIZE - coreLocalSpace);
        coreLocalSpace -= size;
        return (void*)addr;
    }

    return NULL;
}

PML_Instance
VirtMemMan_CreateInstance(void) {
    PML_Instance pml = (void*)MemMan_Alloc();
    pml = (PML_Instance)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pml);
    memset ((void*)pml, 0, KiB(4));

    pml[511] = (uint64_t)kernel_pdpt_paddr;
    MARK_PRESENT(pml[511]);
    MARK_WRITE(pml[511]);
    SET_CACHEMODE(pml[511], MEM_TYPE_WB);

    return pml;
}


PML_Instance
VirtMemMan_SetCurrent(PML_Instance instance) {


    //Update the previous PML instance
    PML_Instance tmp = virtMemData->curPML;

    //NOTE: Unmap first, then map so everything stays the same if tmp = instance
    VirtMemMan_Unmap(tmp,
                     CORE_LOCAL_MEM_ADDR,
                     APLS_SIZE);

    VirtMemMan_Map(instance,
                   CORE_LOCAL_MEM_ADDR,
                   (uint64_t)virtMemData->coreLocalStorage,
                   APLS_SIZE,
                   TRUE,
                   MEM_TYPE_WB,
                   MEM_WRITE | MEM_READ | MEM_EXEC,
                   MEM_KERNEL);

    __asm__ volatile("mov %0, %%cr3" :: "r"(VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), (void*)instance)));

    virtMemData->curPML = instance;

    return tmp;
}

PML_Instance
VirtMemMan_GetCurrent(void) {
    PML_Instance ret = virtMemData->curPML;
    return ret;
}


static void
VirtMemMan_SetupPDPTEntry(PML_Instance inst,
                          uint32_t     pml_off) {
    if((GET_ADDR_4KB(inst[pml_off]) == 0)) {
        uint64_t entry = MemMan_Alloc();
        MARK_PRESENT(entry);
        MARK_WRITE(entry);
        MARK_USER(entry);
        SET_CACHEMODE(entry, MEM_TYPE_WB);
        inst[pml_off] = entry;
    }
}

static void
VirtMemMan_SetupPDEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off) {
    VirtMemMan_SetupPDPTEntry(inst, pml_off);
    uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress (CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));

    if((GET_ADDR_4KB(pdpt[pdpt_off]) == 0) | (GET_PSE(pdpt[pdpt_off]) == 1)) {
        pdpt[pdpt_off] = MemMan_Alloc();
        MARK_PRESENT(pdpt[pdpt_off]);
        MARK_WRITE(pdpt[pdpt_off]);
        MARK_USER(pdpt[pdpt_off]);
        SET_CACHEMODE(pdpt[pdpt_off], MEM_TYPE_WB);
    }
}

static void
VirtMemMan_SetupPTEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off,
                        uint32_t     pd_off) {
    VirtMemMan_SetupPDEntry(inst, pml_off, pdpt_off);

    uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress (CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

    if((GET_ADDR_4KB(pd[pd_off]) == 0) | (GET_PSE(pd[pd_off]) == 1)) {
        pd[pd_off] = MemMan_Alloc();
        MARK_PRESENT(pd[pd_off]);
        MARK_WRITE(pd[pd_off]);
        MARK_USER(pd[pd_off]);
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
                    MEM_SECURITY_PERMS sec_perms) {
    if(phys_addr % GiB(1) | virt_addr % GiB(1))
        __asm__ ("cli\n\thlt");

    if(!virtMemData->hugePageSupport) {
        for(uint64_t i = 0; i < 512; i++) {
            VirtMemMan_MapLPage(inst,
                                virt_addr + i * MiB(2),
                                phys_addr + i * MiB(2),
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        }
        return;
    }

    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;

    VirtMemMan_SetupPDPTEntry(inst, pml_off);

    uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    pdpt[pdpt_off] = phys_addr;
    if(present)MARK_PRESENT(pdpt[pdpt_off]);
    else MARK_ABSENT(pdpt[pdpt_off]);

    MARK_PSE(pdpt[pdpt_off]);
    SET_CACHEMODE(pdpt[pdpt_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
    else MARK_NX(pdpt[pdpt_off]);

    if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

    if(inst == virtMemData->curPML) {
        __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
    }
}

void
VirtMemMan_MapLPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms) {

    if(phys_addr % MiB(2) | virt_addr % MiB(2))
        __asm__ ("cli\n\thlt");

    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;

    VirtMemMan_SetupPDEntry(inst, pml_off, pdpt_off);

    uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

    pd[pd_off] = phys_addr;
    if(present)MARK_PRESENT(pd[pd_off]);
    else MARK_ABSENT(pd[pd_off]);

    MARK_PSE(pd[pd_off]);
    SET_CACHEMODE(pd[pd_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pd[pd_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pd[pd_off]);
    else MARK_NX(pd[pd_off]);

    if(sec_perms & MEM_USER)MARK_USER(pd[pd_off]);


    if(inst == virtMemData->curPML) {
        __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
    }
}


void
VirtMemMan_MapSPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms) {

    if(phys_addr % KiB(4) | virt_addr % KiB(4))
        __asm__ ("cli\n\thlt");

    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;
    uint32_t pt_off = (virt_addr >> 12) & 0x1FF;

    VirtMemMan_SetupPTEntry(inst, pml_off, pdpt_off, pd_off);

    uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));
    uint64_t *pt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_off]));

    pt[pt_off] = phys_addr;
    if(present)MARK_PRESENT(pt[pt_off]);
    else MARK_ABSENT(pt[pt_off]);

    SET_CACHEMODE(pt[pt_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pt[pt_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pt[pt_off]);
    else MARK_NX(pt[pt_off]);

    if(sec_perms & MEM_USER)MARK_USER(pt[pt_off]);

    if(inst == virtMemData->curPML) {
        __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
    }
}

void
VirtMemMan_UnmapSPage(PML_Instance inst, uint64_t virt_addr) {
    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;
    uint32_t pt_off = (virt_addr >> 12) & 0x1FF;

    if(GET_ADDR_4KB(inst[pml_off]) != 0) {
        uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
        if(GET_ADDR_4KB(pdpt[pdpt_off]) != 0) {
            uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

            if(GET_ADDR_4KB(pd[pd_off]) != 0) {
                uint64_t *pt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_off]));

                pt[pt_off] = 0;
                if(inst == virtMemData->curPML) {
                    __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
                }
            }
        }
    }
}


void
VirtMemMan_UnmapLPage(PML_Instance inst, uint64_t virt_addr) {
    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;

    if(GET_ADDR_4KB(inst[pml_off]) != 0) {
        uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
        if(GET_ADDR_4KB(pdpt[pdpt_off]) != 0) {
            uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));


            pd[pd_off] = 0;
            if(inst == virtMemData->curPML) {
                __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
            }
        }
    }
}

void
VirtMemMan_UnmapHPage(PML_Instance inst, uint64_t virt_addr) {
    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    if(GET_ADDR_4KB(inst[pml_off]) != 0) {
        uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));

        pdpt[pdpt_off] = 0;
        if(inst == virtMemData->curPML) {
            __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
        }
    }
}

void
VirtMemMan_Unmap(PML_Instance inst,
                 uint64_t virt_addr,
                 uint64_t size) {
    if(size % KiB(4))
        size += KiB(4) - (size % KiB(4));

    while(size > 0) {
        if(size == KiB(4) && virt_addr % KiB(4) == 0) {
            size -= KiB(4);
            VirtMemMan_UnmapSPage(inst,
                                  virt_addr);
        } else if(size == MiB(2) && virt_addr % MiB(2) == 0) {
            size -= MiB(2);
            VirtMemMan_UnmapLPage(inst,
                                  virt_addr);
        } else if(size == GiB(1) && virt_addr % GiB(1) == 0) {
            size -= GiB(1);
            VirtMemMan_UnmapHPage(inst,
                                  virt_addr);
        } else if(size >= GiB(1) && virt_addr % GiB(1) == 0) {
            size -= GiB(1);
            VirtMemMan_UnmapHPage(inst,
                                  virt_addr);
            virt_addr += GiB(1);
        } else if(size >= MiB(2) && virt_addr % MiB(2) == 0) {
            size -= MiB(2);
            VirtMemMan_UnmapLPage(inst,
                                  virt_addr);
            virt_addr += MiB(2);
        } else if(size >= KiB(4) && virt_addr % KiB(4) == 0) {
            size -= KiB(4);
            VirtMemMan_UnmapSPage(inst,
                                  virt_addr);
            virt_addr += KiB(4);
        } else break;   //Can't determine a mapping, just stop
    }
}

void
VirtMemMan_Map(PML_Instance       inst,
               uint64_t           virt_addr,
               uint64_t           phys_addr,
               uint64_t           size,
               bool               present,
               MEM_TYPES          cache,
               MEM_ACCESS_PERMS   access_perm,
               MEM_SECURITY_PERMS sec_perms) {
    //Determine the generally best mapping for the given size
    if(size % KiB(4) != 0)
        size += KiB(4) - (size % KiB(4));

    if(phys_addr % KiB(4) != 0 | virt_addr % KiB(4) != 0)
        __asm__ volatile("cli\n\thlt" :: "a"(phys_addr), "b"(virt_addr));

    while(size > 0) {
        if(size == KiB(4) && virt_addr % KiB(4) == 0 && phys_addr % KiB(4) == 0) {
            size -= KiB(4);
            VirtMemMan_MapSPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        } else if(size == MiB(2) && virt_addr % MiB(2) == 0 && phys_addr % MiB(2) == 0) {
            size -= MiB(2);
            VirtMemMan_MapLPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        } else if(size == GiB(1) && virt_addr % GiB(1) == 0 && phys_addr % GiB(1) == 0) {
            size -= GiB(1);
            VirtMemMan_MapHPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        } else if(size >= GiB(1) && virt_addr % GiB(1) == 0 && phys_addr % GiB(1) == 0) {
            size -= GiB(1);
            VirtMemMan_MapHPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
            virt_addr += GiB(1);
            phys_addr += GiB(1);
        } else if(size >= MiB(2) && virt_addr % MiB(2) == 0 && phys_addr % MiB(2) == 0) {
            size -= MiB(2);
            VirtMemMan_MapLPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
            virt_addr += MiB(2);
            phys_addr += MiB(2);
        } else if(size >= KiB(4) && virt_addr % KiB(4) == 0 && phys_addr % KiB(4) == 0) {
            size -= KiB(4);
            VirtMemMan_MapSPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
            virt_addr += KiB(4);
            phys_addr += KiB(4);
        } else break;	//Can't determine a mapping, just stop
    }
}

void*
VirtMemMan_GetPhysicalAddress(PML_Instance  inst,
                              void         *addr) {
    //Parse the paging structures to determine the physical address
    uint64_t p_addr = (uint64_t)addr;

    uint32_t pml_off = (p_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (p_addr >> 30) & 0x1FF;
    uint32_t pd_off = (p_addr >> 21) & 0x1FF;
    uint32_t pt_off = (p_addr >> 12) & 0x1FF;

    if(inst[pml_off] & 1) {
        uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));

        if(((pdpt[pdpt_off] >> 7) & 1) && (pdpt[pdpt_off] & 1)) { //Check for PSE
            return (void*)(GET_ADDR_1GB(pdpt[pdpt_off]) + (p_addr % GiB(1)));
        }

        if(pdpt[pdpt_off] & 1) {
            uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

            if(((pd[pd_off] >> 7) & 1) && (pd[pd_off] & 1)) { //Check for PSE
                return (void*)(GET_ADDR_2MB(pd[pd_off]) + (p_addr % MiB(2)));
            }

            if(pd[pd_off] & 1) {

                uint64_t *pt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_off]));

                if(pt[pt_off] & 1) {
                    return (void*)(GET_ADDR_4KB(pt[pt_off]) + (p_addr % KiB(4)));
                }
            }
        }

    }

    return NULL;
}

void*
VirtMemMan_GetVirtualAddress(CachingMode c,
                             void *addr) {

    if(c == CachingModeWriteBack)return (void*)((uint64_t)addr + 0xfffffffc00000000ull + (((uint64_t)addr < 0x100000000)?0x200000000:0));
    else if(c == CachingModeWriteThrough && (uint64_t)addr < 0x100000000)return (void*)((uint64_t)addr + 0xfffffffc00000000);
    else if(c == CachingModeUncachable   && (uint64_t)addr < 0x100000000)return (void*)((uint64_t)addr + 0xfffffffb00000000);

    return NULL;
}

static void
VirtMemMan_GetMapping(uint64_t *pml, uint64_t *pdpt, MemoryAllocationType allocType, MEM_SECURITY_PERMS sec_perms) {

    int pml_base = 256;
    int pdpt_base = 0;

    switch(allocType) {
    case MemoryAllocationType_Heap:
        pml_base = 511;
        break;
    default:
        pml_base = 256;
        break;
    }

    if(sec_perms & MEM_USER) {

        switch(allocType) {
        case MemoryAllocationType_Heap:
            pml_base = 1;
            break;
        case MemoryAllocationType_Application:
            pml_base = 20;
            break;
        case MemoryAllocationType_Stack:
            pml_base = 2;
            break;
        case MemoryAllocationType_MMap:
            pml_base = 3;
            break;
        case MemoryAllocationType_MMapLo:
            pml_base = 0;
            pdpt_base = 1;
            break;
        case MemoryAllocationType_ApplicationProtected:
            pml_base = 21;
            break;
        default:
            pml_base = 0;
            pdpt_base = 1;
            break;
        }
    }

    if(pml != NULL)*pml = pml_base;
    if(pdpt != NULL)*pdpt = pdpt_base;
}

uint64_t
VirtMemMan_GetAllocTypeBase(MemoryAllocationType allocType,
                            MEM_SECURITY_PERMS sec_perms) {
    uint64_t pml_base = 256;
    uint64_t pdpt_base = 0;


    allocType = allocType & ~MemoryAllocationType_Global;

    VirtMemMan_GetMapping(&pml_base, &pdpt_base, allocType, sec_perms);

    return (pml_base << 39) + (pdpt_base << 30);
}

uint64_t
VirtMemMan_GetAllocTypeTop(MemoryAllocationType allocType,
                           MEM_SECURITY_PERMS sec_perms) {
    uint64_t pml_base = 256;
    uint64_t pdpt_base = 0;

    allocType = allocType & ~MemoryAllocationType_Global;

    VirtMemMan_GetMapping(&pml_base, &pdpt_base, allocType, sec_perms);

    return (pml_base + 1) << 39;
}

uint64_t
VirtMemMan_FindFreeAddress(PML_Instance       inst,
                           uint64_t           *vaddr,
                           uint64_t           size,
                           MemoryAllocationType allocType,
                           MEM_SECURITY_PERMS sec_perms) {
    if(size % KiB(4) != 0)
        size = (size/KiB(4) + 1) * KiB(4);	//Align the size to higher 4KiB

    uint64_t needed_score = size;
    uint64_t cur_score = 0;

    uint64_t prev_val = 0;

    allocType = allocType & ~MemoryAllocationType_Global;

    uint64_t addr = 0;
#define BUILD_ADDR(pml, pdpt, pd, pt) if(cur_score == 0)(addr = pml << 39 | pdpt << 30 | pd << 21 | pt << 12)

    uint64_t pml_base = 256;
    uint64_t pdpt_base = 0;

    VirtMemMan_GetMapping(&pml_base, &pdpt_base, allocType, sec_perms);


    for(uint64_t pml_i = pml_base; pml_i < (uint64_t)(pml_base + 128) && pml_i < 512 && cur_score < needed_score; ++pml_i) {
        if((inst[pml_i] & 1) == 0) {
            //Check the pml4 table for free entries if more than 256GiB of space is requested
            BUILD_ADDR(pml_i, 0, 0, 0);
            prev_val = pml_i;
            cur_score += GiB(512);
        } else if((inst[pml_i] & 1) == 1 && GET_PSE(inst[pml_i]) == 0) {

            uint64_t *pdpt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_i]));

            //Reset the score if a block was skipped to keep the allocation continuous
            if(cur_score > 0 && pml_i != 0 && (pml_i - 1) != prev_val)cur_score = 0;

            for(uint64_t pdpt_i = pdpt_base; pdpt_i < 512 && cur_score < needed_score; ++pdpt_i) {

                if(((pdpt[pdpt_i] & 1) == 0)) {
                    //Check the pdpt table if more than 1GiB of space is requested
                    BUILD_ADDR(pml_i, pdpt_i, 0, 0);
                    cur_score += GiB(1);
                    prev_val = pdpt_i;

                } else if((pdpt[pdpt_i] & 1) == 1 && GET_PSE(pdpt[pdpt_i]) == 0) {

                    uint64_t *pd = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_i]));

                    //Reset the score if a block was skipped
                    if(cur_score > 0 && pdpt_i != 0 && (pdpt_i - 1) != prev_val)cur_score = 0;

                    for(uint64_t pd_i = 0; pd_i < 512 && cur_score < needed_score; ++pd_i) {

                        if((pd[pd_i] & 1) == 0) {

                            //Check the pd table if more than 2MiB of space is requested
                            BUILD_ADDR(pml_i, pdpt_i, pd_i, 0);
                            cur_score += MiB(2);
                            prev_val = pd_i;

                        } else if((pd[pd_i] & 1) == 1 && GET_PSE(pd[pd_i]) == 0) {

                            uint64_t *pt = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_i]));

                            //Reset the score if a block was skipped
                            if(cur_score > 0 && pd_i != 0 && (pd_i - 1) != prev_val)cur_score = 0;

                            for(uint64_t pt_i = 0; pt_i < 512 && cur_score < needed_score; ++pt_i) {
                                if(pt_i == 0 && pd_i == 0 && pdpt_i == 0 && pml_i == 0)pt_i = 1;
                                //Reset the score if a block was skipped
                                if(pt_i != 0 && prev_val != (pt_i - 1))cur_score = 0;

                                if((pt[pt_i] & 1) == 0) {
                                    //Check the pt table for 4KiB pages
                                    BUILD_ADDR(pml_i, pdpt_i, pd_i, pt_i);
                                    cur_score += KiB(4);
                                    prev_val = pt_i;
                                }
                            }
                        }
                    }
                }
            }
        } else BUILD_ADDR(pml_i, 0, 0, 0);
    }

    //Apply sign extension
    if((addr >> 47) & 1)addr |= 0xffff000000000000;

#undef BUILD_ADDR
    *vaddr = addr;
    if(addr == 0)
        return -1;
    return 0;
}


void
VirtMemMan_ReturnPermissions(uint64_t entry,
                             MEM_TYPES *cacheType,
                             MEM_ACCESS_PERMS *access_perm,
                             MEM_SECURITY_PERMS *security_perm) {


    MEM_ACCESS_PERMS access = 0;
    MEM_SECURITY_PERMS sec = 0;
    MEM_TYPES cache = 0;

    if(GET_PRESENT(entry)) {

        access |= MEM_READ;

        if(GET_USER(entry))
            sec |= MEM_USER;
        else
            sec |= MEM_KERNEL;

        if(GET_WRITE(entry))
            access |= MEM_WRITE;

        if(!GET_NX(entry))
            access |= MEM_EXEC;

        cache = GET_CACHEMODE(entry);
    }

    if(cacheType != NULL)*cacheType = cache;
    if(access_perm != NULL)*access_perm = access;
    if(security_perm != NULL)*security_perm = sec;
}

void
VirtMemMan_GetAddressPermissions(PML_Instance inst,
                                 uint64_t addr,
                                 MEM_TYPES *cacheType,
                                 MEM_ACCESS_PERMS *access_perm,
                                 MEM_SECURITY_PERMS *security_perm) {
#define GET_PERM(x) return VirtMemMan_ReturnPermissions(x, cacheType, access_perm, security_perm)

    uint32_t pml_off = (addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (addr >> 30) & 0x1FF;
    uint32_t pd_off = (addr >> 21) & 0x1FF;
    uint32_t pt_off = (addr >> 12) & 0x1FF;

    if(cacheType != NULL)*cacheType = 0;
    if(access_perm != NULL)*access_perm = 0;
    if(security_perm != NULL)*security_perm = 0;

    uint64_t pml_paddr = GET_ADDR_4KB(inst[pml_off]);
    uint64_t *pml_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pml_paddr);
    if(pml_paddr == 0)return;

    uint64_t pdpt_paddr = pml_vaddr[pdpt_off];
    if(GET_PSE(pdpt_paddr))
        GET_PERM(pdpt_paddr);
    else
        pdpt_paddr = GET_ADDR_4KB(pdpt_paddr);

    if(pdpt_paddr == 0)return;
    uint64_t *pdpt_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pdpt_paddr);


    uint64_t pd_paddr = pdpt_vaddr[pd_off];

    if(GET_PSE(pd_paddr))
        GET_PERM(pd_paddr);
    else
        pd_paddr = GET_ADDR_4KB(pd_paddr);

    if(pd_paddr == 0)return;
    uint64_t *pd_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pd_paddr);

    GET_PERM(pd_vaddr[pt_off]);

#undef GET_PERM
}

void
VirtMemMan_FreePageTable(PML_Instance inst) {
    for(int pml_i = 0; pml_i < 512; pml_i++) {
        uint64_t pml_paddr = GET_ADDR_4KB(inst[pml_i]);
        uint64_t *pml_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pml_paddr);
        if(pml_paddr == 0)continue;

        for(int pdpt_i = 0; pdpt_i < 512; pdpt_i++) {
            uint64_t pdpt_paddr = pml_vaddr[pdpt_i];
            if(GET_PSE(pdpt_paddr))
                continue;
            else
                pdpt_paddr = GET_ADDR_4KB(pdpt_paddr);

            uint64_t *pdpt_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pdpt_paddr);

            if(pdpt_paddr == 0)continue;

            for(int pd_i = 0; pd_i < 512; pd_i++) {
                uint64_t pd_paddr = pdpt_vaddr[pd_i];

                if(GET_PSE(pd_paddr))
                    continue;
                else
                    pd_paddr = GET_ADDR_4KB(pd_paddr);

                if(pd_paddr == 0)continue;

                MemMan_Free(pd_paddr);
            }

            MemMan_Free(pdpt_paddr);

        }

        MemMan_Free(pml_paddr);
    }
}

void
VirtMemMan_HandlePageFault(uint32_t UNUSED(int_no),
                           uint32_t err_code) {
    uint64_t addr = 0;
    __asm__ volatile("movq %%cr2, %0" : "=r"(addr) ::);

    MemoryAllocationFlags flags = 0;
    flags |= (err_code & 1) ? MemoryAllocationFlags_Present : MemoryAllocationFlags_NotPresent;
    flags |= (err_code & (1 << 1)) ? MemoryAllocationFlags_Write : MemoryAllocationFlags_Read;
    flags |= (err_code & (1 << 2)) ? MemoryAllocationFlags_User : MemoryAllocationFlags_Kernel;
    flags |= (err_code & (1 << 4)) ? MemoryAllocationFlags_Exec : MemoryAllocationFlags_NoExec;

    Registers *regs = GetSavedInterruptState();

    HandlePageFault(addr,
                    regs->rip,
                    flags);

    __asm__ volatile("movq %0, %%cr2" :: "r"((uint64_t)0) : );
}


uint64_t
VirtMemMan_GetPageSize(PML_Instance inst,
                       uint64_t vaddr) {
    uint32_t pml_off = (vaddr >> 39) & 0x1FF;
    uint32_t pdpt_off = (vaddr >> 30) & 0x1FF;
    uint32_t pd_off = (vaddr >> 21) & 0x1FF;
    uint32_t pt_off = (vaddr >> 12) & 0x1FF;

    uint64_t pml_paddr = GET_ADDR_4KB(inst[pml_off]);
    uint64_t *pml_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pml_paddr);
    if(pml_paddr == 0)return -1;

    uint64_t pdpt_paddr = pml_vaddr[pdpt_off];
    if(GET_PSE(pdpt_paddr))
        return GiB(1);
    else
        pdpt_paddr = GET_ADDR_4KB(pdpt_paddr);

    if(pdpt_paddr == 0)return -1;
    uint64_t *pdpt_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pdpt_paddr);


    uint64_t pd_paddr = pdpt_vaddr[pd_off];

    if(GET_PSE(pd_paddr))
        return MiB(2);
    else
        pd_paddr = GET_ADDR_4KB(pd_paddr);

    if(pd_paddr == 0)return -1;
    uint64_t *pd_vaddr = (uint64_t*)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)pd_paddr);

    return KiB(4);
}