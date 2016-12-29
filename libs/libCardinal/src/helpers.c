#include "helpers.h"


int
__card_setup_shmem(uint64_t *len,
                   MemoryAllocationFlags target,
                   uint64_t *key,
                   uint64_t *vAddress) {

    if(*len % PAGE_SIZE)
        *len += PAGE_SIZE - *len % PAGE_SIZE;

    if(AllocateSharedMemory(*len,
                            CachingModeWriteBack,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                            vAddress) != 0)
        return -1;

    if(GetSharedMemoryKey(*vAddress,
                          *len,
                          CachingModeWriteBack,
                          target | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                          key) != 0) {
        Unmap(*vAddress, *len);
        return -1;
    }

    memset((void*)*vAddress, 0, *len);

    return 0;
}