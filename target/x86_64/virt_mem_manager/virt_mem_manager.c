#include "types.h"
#include "page_manager/phys_mem_manager.h"
#include "utils/native.h"
#include "common/common.h"
#include "virt_mem_manager.h"
#include "CPUID/cpuid.h"
#include "managers.h"

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

#define GET_ADDR_4KB(a) (a & ~0xf000000000000fff)
#define GET_ADDR_2MB(a) (a & ~0xf0000000000fffff)
#define GET_ADDR_1GB(a) (a & ~0xf00000003fffffff)

#define CORE_LOCAL_MEM_ADDR (0xffffff0000000000)

uint64_t* kernel_pdpt = NULL;

typedef struct VirtMemManData {
    PML_Instance curPML;
    bool hugePageSupport;
    uint64_t *coreLocal_pdpt;
} VirtMemManData;

static uint64_t coreLocalSpace;
static VirtMemManData *virtMemData;

void
VirtMemMan_InitializeBootstrap(void) {
    virtMemData = bootstrap_malloc(sizeof(VirtMemManData));
    virtMemData->curPML = (uint64_t*)0xfffffffe00001000;    //Where initial PML is located
    virtMemData->hugePageSupport = FALSE;
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
    pat |= ((uint64_t)0x1) << 24;  //PAT3
    wrmsr(PAT_MSR, pat);

    //Allocate a new PML4 and set of pages
    PML_Instance pml = (void*)MemMan_Alloc ();
    pml = (PML_Instance)GetVirtualAddress(CachingModeWriteBack, (void*)pml);

    memset ((void*)pml, 0, KiB(4));

    if(kernel_pdpt == NULL) {
        void* pdpt_0 = (void*)MemMan_Alloc ();


        kernel_pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, pdpt_0);
        pml[511] = (uint64_t)pdpt_0 | 3;    //Keep the top 512GiB of memory mapped into all address spaces
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
    } else {

        void* pdpt_0 = GetPhysicalAddress(kernel_pdpt);
        pml[511] = (uint64_t)pdpt_0 | 3;
    }

    //Setup core specific memory
    VirtMemMan_Map(pml,
                   CORE_LOCAL_MEM_ADDR,
                   MemMan_Alloc4KiBPageCont(APLS_SIZE/PAGE_SIZE),
                   APLS_SIZE,
                   TRUE,
                   MEM_TYPE_WB,
                   MEM_READ | MEM_WRITE,
                   MEM_KERNEL);

    wrmsr(0xC0000080, rdmsr(0xC0000080) | (1 << 11));

    virtMemData->coreLocal_pdpt = (uint64_t*)pml[(CORE_LOCAL_MEM_ADDR >> 39) & 0x1FF];


    VirtMemMan_SetCurrent(pml);

    //Now change the virtMemData pointer to refer to the TLS version of the structure
    VirtMemManData* tmp = virtMemData;
    virtMemData = (VirtMemManData*)CORE_LOCAL_MEM_ADDR;
    virtMemData->coreLocal_pdpt = (uint64_t*)pml[(CORE_LOCAL_MEM_ADDR >> 39) & 0x1FF];
    virtMemData->curPML = tmp->curPML;
    virtMemData->hugePageSupport = tmp->hugePageSupport;
    coreLocalSpace = APLS_SIZE - sizeof(VirtMemManData);

    //__asm__ volatile("mov %0, %%rax\n\thlt" :: "ra"((uint64_t)&virtMemData->curPML));
    //Enable the NX bit
}

void*
VirtMemMan_AllocCoreLocalData(uint64_t size) {
    if(size <= coreLocalSpace && coreLocalSpace > 0) {
        uint64_t addr = (uint64_t)virtMemData + (APLS_SIZE - coreLocalSpace);
        coreLocalSpace -= size;
        return (void*)addr;
    }

    return NULL;
}

PML_Instance
VirtMemMan_CreateInstance(void) {
    PML_Instance pml = (void*)MemMan_Alloc();
    pml = (PML_Instance)GetVirtualAddress(CachingModeWriteBack, (void*)pml);
    memset ((void*)pml, 0, KiB(4));

    pml[511] = (uint64_t)GetPhysicalAddress(kernel_pdpt);
    MARK_PRESENT(pml[511]);
    MARK_WRITE(pml[511]);
    SET_CACHEMODE(pml[511], MEM_TYPE_WB);

    return pml;
}

PML_Instance
VirtMemMan_SetCurrent(PML_Instance instance) {
    PML_Instance tmp = virtMemData->curPML;

    //Setup the thread local storage for this core before changing!
    uint64_t *pml = (uint64_t*)instance;
    pml[(CORE_LOCAL_MEM_ADDR >> 39) & 0x1FF] = (uint64_t)virtMemData->coreLocal_pdpt;

    __asm__ volatile("mov %0, %%cr3" :: "r"(GetPhysicalAddress(instance)));

    virtMemData->curPML = instance;
    return tmp;
}

PML_Instance
VirtMemMan_GetCurrent(void) {
    return virtMemData->curPML;
}


static void
VirtMemMan_SetupPDPTEntry(PML_Instance inst,
                          uint32_t     pml_off) {
    if(GET_ADDR_4KB(inst[pml_off]) == 0) {
        uint64_t entry = MemMan_Alloc();
        MARK_PRESENT(entry);
        MARK_WRITE(entry);
        SET_CACHEMODE(entry, MEM_TYPE_WB);
        inst[pml_off] = entry;
        __asm__ volatile("invlpg (%0)" :: "r"(GET_ADDR_4KB(inst[pml_off])));
    }
}

static void
VirtMemMan_SetupPDEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off) {
    VirtMemMan_SetupPDPTEntry(inst, pml_off);
    uint64_t *pdpt = (uint64_t*)GetVirtualAddress (CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));

    if(GET_ADDR_4KB(pdpt[pdpt_off]) == 0) {
        pdpt[pdpt_off] = MemMan_Alloc();
        MARK_PRESENT(pdpt[pdpt_off]);
        MARK_WRITE(pdpt[pdpt_off]);
        SET_CACHEMODE(pdpt[pdpt_off], MEM_TYPE_WB);
        __asm__ volatile("invlpg (%0)" :: "r"(GET_ADDR_4KB(pdpt[pdpt_off])));
    }
}

static void
VirtMemMan_SetupPTEntry(PML_Instance inst,
                        uint32_t     pml_off,
                        uint32_t     pdpt_off,
                        uint32_t     pd_off) {
    VirtMemMan_SetupPDEntry(inst, pml_off, pdpt_off);

    uint64_t *pdpt = (uint64_t*)GetVirtualAddress (CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

    if(GET_ADDR_4KB(pd[pd_off]) == 0) {
        pd[pd_off] = MemMan_Alloc();
        MARK_PRESENT(pd[pd_off]);
        MARK_WRITE(pd[pd_off]);
        SET_CACHEMODE(pd[pd_off], MEM_TYPE_WB);
        __asm__ volatile("invlpg (%0)" :: "r"(GET_ADDR_4KB(pd[pd_off])));
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
    phys_addr = phys_addr/GiB(1) * GiB(1);  //Align the physical address

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

    uint64_t *pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    pdpt[pdpt_off] = phys_addr;
    if(present)MARK_PRESENT(pdpt[pdpt_off]);

    MARK_PSE(pdpt[pdpt_off]);
    SET_CACHEMODE(pdpt[pdpt_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pdpt[pdpt_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pdpt[pdpt_off]);
    else MARK_NX(pdpt[pdpt_off]);

    if(sec_perms & MEM_USER)MARK_USER(pdpt[pdpt_off]);

    if(inst == virtMemData->curPML)__asm__ volatile("invlpg (%0)" :: "a"(virt_addr));
}

void
VirtMemMan_MapLPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms) {
    phys_addr = phys_addr/MiB(2) * MiB(2);	//Align the physical address

    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;

    VirtMemMan_SetupPDEntry(inst, pml_off, pdpt_off);

    uint64_t *pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));


    pd[pd_off] = phys_addr;
    if(present)MARK_PRESENT(pd[pd_off]);

    MARK_PSE(pd[pd_off]);
    SET_CACHEMODE(pd[pd_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pd[pd_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pd[pd_off]);
    else MARK_NX(pd[pd_off]);

    if(sec_perms & MEM_USER)MARK_USER(pd[pd_off]);


    if(inst == virtMemData->curPML)__asm__ volatile("invlpg (%0)" :: "a"(virt_addr));
}


void
VirtMemMan_MapSPage(PML_Instance       inst,
                    uint64_t           virt_addr,
                    uint64_t           phys_addr,
                    bool               present,
                    MEM_TYPES          cache,
                    MEM_ACCESS_PERMS   access_perm,
                    MEM_SECURITY_PERMS sec_perms) {
    phys_addr = phys_addr/KiB(4) * KiB(4);	//Align the physical address

    uint32_t pml_off = (virt_addr >> 39) & 0x1FF;
    uint32_t pdpt_off = (virt_addr >> 30) & 0x1FF;
    uint32_t pd_off = (virt_addr >> 21) & 0x1FF;
    uint32_t pt_off = (virt_addr >> 12) & 0x1FF;

    VirtMemMan_SetupPTEntry(inst, pml_off, pdpt_off, pd_off);

    uint64_t *pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));
    uint64_t *pd = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));
    uint64_t *pt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_off]));

    if(virt_addr == 0xfffffffb08800000)__asm__ ("cli\n\thlt" :: "a"(pt[pt_off]));
    pt[pt_off] = phys_addr;
    if(present)MARK_PRESENT(pt[pt_off]);

    SET_CACHEMODE(pt[pt_off], cache);
    if(access_perm & MEM_WRITE)MARK_WRITE(pt[pt_off]);

    if(access_perm & MEM_EXEC)MARK_EXEC(pt[pt_off]);
    else MARK_NX(pt[pt_off]);

    if(sec_perms & MEM_USER)MARK_USER(pt[pt_off]);

    if(inst == virtMemData->curPML){
    __asm__ volatile("invlpg (%0)" :: "r"(virt_addr));
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

    while(size > 0) {
        if(size == KiB(4)) {
            size -= KiB(4);
            VirtMemMan_MapSPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        } else if(size == MiB(2)) {
            size -= MiB(2);
            VirtMemMan_MapLPage(inst,
                                virt_addr,
                                phys_addr,
                                present,
                                cache,
                                access_perm,
                                sec_perms);
        } else if(size == GiB(1)) {
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
            VirtMemMan_Map(inst,
                           virt_addr,
                           phys_addr,
                           GiB(1),
                           present,
                           cache,
                           access_perm,
                           sec_perms);
            virt_addr += GiB(1);
            phys_addr += GiB(1);
        } else if(size >= MiB(2) && virt_addr % MiB(2) == 0 && phys_addr % MiB(2) == 0) {
            size -= MiB(2);
            VirtMemMan_Map(inst,
                           virt_addr,
                           phys_addr,
                           MiB(2),
                           present,
                           cache,
                           access_perm,
                           sec_perms);
            virt_addr += MiB(2);
            phys_addr += MiB(2);
        } else if(size >= KiB(4) && virt_addr % KiB(4) == 0 && phys_addr % KiB(4) == 0) {
            size -= KiB(4);
            VirtMemMan_Map(inst,
                           virt_addr,
                           phys_addr,
                           KiB(4),
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
        uint64_t *pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_off]));

        if(((pdpt[pdpt_off] >> 7) & 1) && (pdpt[pdpt_off] & 1)) { //Check for PSE
            return (void*)(GET_ADDR_1GB(pdpt[pdpt_off]) + (p_addr % GiB(1)));
        }

        if(pdpt[pdpt_off] & 1) {
            uint64_t *pd = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_off]));

            if(((pd[pd_off] >> 7) & 1) && (pd[pd_off] & 1)) { //Check for PSE
                return (void*)(GET_ADDR_2MB(pd[pd_off]) + (p_addr % MiB(2)));
            }

            if(pd[pd_off] & 1) {

                uint64_t *pt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_off]));

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

void*
VirtMemMan_FindFreeAddress(PML_Instance       inst,
                           uint64_t           size,
                           MemoryAllocationType allocType,
                           MEM_SECURITY_PERMS sec_perms) {
    if(size % KiB(4) != 0)
        size = (size/KiB(4) + 1) * KiB(4);	//Align the size to higher 4KiB

    uint64_t needed_score = size;
    uint64_t cur_score = 0;

    uint64_t prev_val = 0;

    uint64_t addr = 0;
#define BUILD_ADDR(pml, pdpt, pd, pt) if(cur_score == 0)(addr = pml << 39 | pdpt << 30 | pd << 21 | pt << 12)

    int pml_base = 256;

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
        default:
            pml_base = 0;
            break;
        }
    }


    for(uint64_t pml_i = pml_base; pml_i < (uint64_t)(pml_base + 128) && pml_i < 512 && cur_score < needed_score; ++pml_i) {
        if((inst[pml_i] & 1) == 0 && needed_score >= GiB(256) && needed_score <= GiB(512)) {
            //Check the pml4 table for free entries if more than 256GiB of space is requested
            BUILD_ADDR(pml_i, 0, 0, 0);
            prev_val = pml_i;
            cur_score += GiB(256);
        } else if((inst[pml_i] & 1) == 1) {

            uint64_t *pdpt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(inst[pml_i]));

            //Reset the score if a block was skipped to keep the allocation continuous
            if(cur_score > 0 && pml_i != 0 && (pml_i - 1) != prev_val)cur_score = 0;

            for(uint64_t pdpt_i = 0; pdpt_i < 512 && cur_score < needed_score; ++pdpt_i) {

                if(((pdpt[pdpt_i] & 1) == 0) && (needed_score - cur_score) >= GiB(1)) {
                    //Check the pdpt table if more than 1GiB of space is requested
                    BUILD_ADDR(pml_i, pdpt_i, 0, 0);
                    cur_score += GiB(1);
                    prev_val = pdpt_i;

                } else if((pdpt[pdpt_i] & 1) == 1 && (needed_score - cur_score) < GiB(1)) {

                    uint64_t *pd = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pdpt[pdpt_i]));

                    //Reset the score if a block was skipped
                    if(cur_score > 0 && pdpt_i != 0 && (pdpt_i - 1) != prev_val)cur_score = 0;

                    for(uint64_t pd_i = 0; pd_i < 512 && cur_score < needed_score; ++pd_i) {

                        if(((pd[pd_i] & 1) == 0) && (needed_score - cur_score) >= MiB(2)) {

                            //Check the pd table if more than 2MiB of space is requested
                            BUILD_ADDR(pml_i, pdpt_i, pd_i, 0);
                            cur_score += MiB(2);
                            prev_val = pd_i;

                        } else if((pd[pd_i] & 1) == 1 && (needed_score - cur_score) < MiB(2)) {

                            uint64_t *pt = (uint64_t*)GetVirtualAddress(CachingModeWriteBack, (void*)GET_ADDR_4KB(pd[pd_i]));

                            //Reset the score if a block was skipped
                            if(cur_score > 0 && pd_i != 0 && (pd_i - 1) != prev_val)cur_score = 0;

                            for(uint64_t pt_i = 0; pt_i < 512 && cur_score < needed_score; ++pt_i) {

                                //Reset the score if a block was skipped
                                if(pt_i != 0 && prev_val != (pt_i - 1))cur_score = 0;

                                if((pt[pt_i] & 1) == 0 && (needed_score - cur_score) >= KiB(4)) {
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
    if(addr == 0)addr = KiB(4);
    return (void*)addr;
}
