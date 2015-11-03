#include "kmalloc.h"
#include "managers.h"
#include "block_alloc.h"

static void *_largeH_Loc;
static size_t _largeH_FreeSize;
static BLOCK_TYPE _largeH_Blocks[LARGE_HEAP_BLOCK_COUNT];

static void *_smallH_Loc;
static size_t _smallH_FreeSize;
static BLOCK_TYPE _smallH_Blocks[SMALL_HEAP_BLOCK_COUNT];

static bool initialized = FALSE;
static bool _runtime_initialized = FALSE;

#if ENGINE_DEBUG
static char leakSig[MEMORY_LEAK_CHECK_BLOCK_SIZE];
#endif

void SetRuntimeInitialized(bool status)
{
    _runtime_initialized = status;
}


int Balloc_Initialize()
{
    if (initialized)return 0;
#if ENGINE_DEBUG

    uint32_t addr = virtMemMan_FindEmptyAddress(
                        LARGE_HEAP_MEM_SIZE + LARGE_HEAP_BLOCK_COUNT * MEMORY_LEAK_CHECK_BLOCK_SIZE +
                        SMALL_HEAP_MEM_SIZE + SMALL_HEAP_BLOCK_COUNT * MEMORY_LEAK_CHECK_BLOCK_SIZE,
                        MEM_KERNEL);	//Make sure to allocate enough room for the memory leak check signature

    _smallH_Loc = addr;
    _smallH_FreeSize = SMALL_HEAP_MEM_SIZE;

    addr += SMALL_HEAP_MEM_SIZE + SMALL_HEAP_BLOCK_COUNT * MEMORY_LEAK_CHECK_BLOCK_SIZE;

    _largeH_Loc = addr;
    _largeH_FreeSize = LARGE_HEAP_MEM_SIZE;

    //Generate a signature to use to check for memory leaks
    for (int i = 0; i < MEMORY_LEAK_CHECK_BLOCK_SIZE; i++)
        {
            leakSig[i] = (char)rand(i);
        }
#else
    uint32_t addr = virtMemMan_FindEmptyAddress(LARGE_HEAP_MEM_SIZE + SMALL_HEAP_MEM_SIZE, MEM_KERNEL);

    _smallH_Loc = addr;
    _smallH_FreeSize = SMALL_HEAP_MEM_SIZE;

    _largeH_Loc = addr + SMALL_HEAP_MEM_SIZE;
    _largeH_FreeSize = LARGE_HEAP_MEM_SIZE;
#endif

    //Initialize all blocks to being free
    memset(_smallH_Blocks, BLOCK_TYPE_FREE, SMALL_HEAP_BLOCK_COUNT * sizeof(BLOCK_TYPE));
    memset(_largeH_Blocks, BLOCK_TYPE_FREE, LARGE_HEAP_BLOCK_COUNT * sizeof(BLOCK_TYPE));

    initialized = TRUE;

    return 0;
}

UID
_allocBlockSet(size_t startIndex,
               size_t blockCount,
               BLOCK_TYPE *blockData,
               size_t blockEntries,
               void *baseLoc,
               size_t blockSize)
{
    //Find the first free block, mark it as a start block
    UID blockIndex = startIndex;

    for (; blockIndex < blockEntries && blockData[blockIndex] != BLOCK_TYPE_FREE; blockIndex++);
    if ( (blockIndex == blockEntries) | (blockIndex + blockCount > blockEntries))return -1;	//-1 = Not enough memory

    //Make sure we have the requested number of free blocks
    for (UID i = 0; i < blockCount; i++)
        {
            if (blockData[blockIndex + i] != BLOCK_TYPE_FREE)
                {
                    //If we don't, try again
                    return _allocBlockSet(blockIndex + i + 1, blockCount, blockData, blockEntries, baseLoc, blockSize);
                }
        }

    //We know we have enough space, quickly set all the mid blocks to MID
    if (blockCount > 2)memset(&blockData[blockIndex + 1], BLOCK_TYPE_ALLOC_MID, (blockCount - 2) * sizeof(BLOCK_TYPE));

    //Setup the start and end of each block
    blockData[blockIndex] = BLOCK_TYPE_ALLOC_START;
    if (blockCount >= 2)blockData[blockIndex + blockCount - 1] = BLOCK_TYPE_ALLOC_END;
    else blockData[blockIndex] = BLOCK_TYPE_ALLOC_START_END;

#if ENGINE_DEBUG
    //Insert leak check signature
    void *loc = (void*)((UID)baseLoc + (blockSize + MEMORY_LEAK_CHECK_BLOCK_SIZE) * blockIndex + blockSize);
    memcpy_s(loc, MEMORY_LEAK_CHECK_BLOCK_SIZE, leakSig, MEMORY_LEAK_CHECK_BLOCK_SIZE);
#endif
    return blockIndex;
}

UID
Balloc_Alloc(size_t size)
{
    UID bid = 0;
    if (!initialized)Balloc_Initialize();

    //Based on the size determine which heap to use
    if (size >= LARGE_HEAP_BLOCK_SIZE)
        {
            //Use large heap
            //Calculate the blockCount
            size_t blockCount = size / LARGE_HEAP_BLOCK_SIZE;
            if (size > LARGE_HEAP_BLOCK_SIZE * blockCount)blockCount++;

            bid = _allocBlockSet(0, blockCount, _largeH_Blocks, LARGE_HEAP_BLOCK_COUNT, _largeH_Loc, LARGE_HEAP_BLOCK_SIZE);
        }
    else
        {
            //Use small heap
            //Calculate the blockCount
            size_t blockCount = size / SMALL_HEAP_BLOCK_SIZE;
            if (size > SMALL_HEAP_BLOCK_SIZE * blockCount)blockCount++;

            bid = _allocBlockSet(0, blockCount, _smallH_Blocks, SMALL_HEAP_BLOCK_COUNT, _smallH_Loc, SMALL_HEAP_BLOCK_SIZE);
            if (bid >= 0)bid += LARGE_HEAP_BLOCK_COUNT;
        }
    return bid;
}

void* Balloc_GetBaseAddress(UID blockID)
{
    //Retrieve the base address of the block at the given UID
    UID blockIndex = blockID;
    void *baseLoc = _largeH_Loc;
    size_t blockSize = LARGE_HEAP_BLOCK_SIZE;

    if (blockIndex >= LARGE_HEAP_BLOCK_COUNT)
        {
            blockIndex -= LARGE_HEAP_BLOCK_COUNT;
            baseLoc = _smallH_Loc;
            blockSize = SMALL_HEAP_BLOCK_SIZE;
        }

#if ENGINE_DEBUG
    blockSize += MEMORY_LEAK_CHECK_BLOCK_SIZE;
#endif

    //If it is a start block calculate its offset assuming that each block before it has a leak check signature
    return (void*)((UID)baseLoc + blockSize * blockIndex);
}

UID Balloc_GetUID(void *baseAddress)
{
    UID addr = (UID)baseAddress;

    size_t blockSize = 0;
#if ENGINE_DEBUG
    blockSize += MEMORY_LEAK_CHECK_BLOCK_SIZE;
#endif

    if (addr >= (UID)_largeH_Loc && addr < (UID)_largeH_Loc + (blockSize + LARGE_HEAP_BLOCK_SIZE) * LARGE_HEAP_BLOCK_COUNT)
        {
            //Given block is part of the large block allocator
            blockSize += LARGE_HEAP_BLOCK_SIZE;
            return (addr - (UID)_largeH_Loc) / blockSize;
        }
    else if (addr >= (UID)_smallH_Loc && addr < (UID)_smallH_Loc + (blockSize + SMALL_HEAP_BLOCK_SIZE) * SMALL_HEAP_BLOCK_COUNT)
        {
            //Given block is part of the small block allocator
            blockSize += SMALL_HEAP_BLOCK_SIZE;
            return LARGE_HEAP_BLOCK_COUNT + (addr - (UID)_smallH_Loc) / blockSize;
        }
    else
        {
            return -1;
        }
}

void Balloc_Free(UID blockID)
{
    //Check leak check signature, log warning if it has changed
    //Mark all blocks until and including end block as free
    UID blockIndex = blockID;
    BLOCK_TYPE *blockType = _largeH_Blocks;
    void *baseLoc = _largeH_Loc;
    size_t blockSize = LARGE_HEAP_BLOCK_SIZE;

    if (blockIndex >= LARGE_HEAP_BLOCK_COUNT)
        {
            blockIndex -= LARGE_HEAP_BLOCK_COUNT;
            blockType = _smallH_Blocks;
            baseLoc = _smallH_Loc;
            blockSize = SMALL_HEAP_BLOCK_SIZE;
        }

    if (blockType[blockIndex] & BLOCK_TYPE_ALLOC_START)
        {
            while (!(blockType[blockIndex] & BLOCK_TYPE_ALLOC_END))
                {
                    blockType[blockIndex] = BLOCK_TYPE_FREE;
                    blockIndex++;
                }

#if ENGINE_DEBUG
            //Check the leak signature for a match
            void *loc = (void*)((UID)baseLoc + (blockSize + MEMORY_LEAK_CHECK_BLOCK_SIZE) * blockIndex + blockSize);
            if (memcmp(loc, leakSig, MEMORY_LEAK_CHECK_BLOCK_SIZE) != 0 && _runtime_initialized)
                {
                    COM_WriteStr("Memory leak detected, UID %x%x", (uint32_t)(blockID >> 32), (uint32_t)(blockID));
                }
#endif

            blockType[blockIndex] = BLOCK_TYPE_FREE;
        }
#if ENGINE_DEBUG
    else
        {
            COM_WriteStr("Invalid Free UID %x%x\r\n", (uint32_t)(blockID >> 32), (uint32_t)(blockID));
        }
#endif
}


void kmalloc_init()
{
    Balloc_Initialize();
}

void* kmalloc(size_t size)
{
    ThreadMan_Lock();

    void *addr = Balloc_GetBaseAddress(Balloc_Alloc(size));
    COM_WriteStr("Addr: %x, %d\r\n", addr, size);
    virtMemMan_PageFaultActionAlloc(TRUE);
    memset(addr, 0, size);
    virtMemMan_PageFaultActionAlloc(FALSE);

    ThreadMan_Unlock();
    return addr;
}

void kfree(void *addr)
{
    ThreadMan_Lock();

    Balloc_Free(Balloc_GetUID(addr));

    ThreadMan_Unlock();
}

void kcompact(void)
{

}