#include "pipe.h"
#include "memory.h"
#include "kmalloc.h"
#include "common.h"
#include "managers.h"

typedef struct {
	uint8_t *DataPool;
	volatile uint64_t WriteLocation;
	volatile uint64_t ReadLocation;
	Spinlock WriteLock;
	Spinlock ReadLock;
	volatile size_t FreeSize;
	volatile uint32_t ReferenceCount;
	size_t Size;
} PipeContext;

UID
CreatePipe(void) {
	PipeContext *pipe = kmalloc(sizeof(PipeContext));
	pipe->Size = pipe->FreeSize = KiB(16);
	pipe->DataPool = AllocateMapping(pipe->Size);
	pipe->WriteLock = CreateSpinlock();
	pipe->ReadLock = CreateSpinlock();
	pipe->WriteLocation = 0;
	pipe->ReadLocation = 0;

	return (UID)pipe;
}
size_t
WritePipe(UID id,
		  const char *data,
		  size_t len) {
	PipeContext *pipe = (PipeContext*)id;
	LockScheduledSpinlock(pipe->WriteLock);

	AtomicIncrement32((uint32_t*)&pipe->ReferenceCount);

	while(pipe->FreeSize == 0) {
		YieldThread();
	}

	uint64_t data_offset = 0;
	size_t write_len = 0;

	while(len != 0){
		size_t freeSpace = pipe->FreeSize;
		
		size_t cpy_len = 0;
		if(freeSpace == 0)
			{
				YieldThread(); 
				continue;
			}
		else if(freeSpace < len)
			cpy_len = freeSpace;
		else 
			cpy_len = len;

		memcpy(pipe->DataPool + pipe->WriteLocation, data + data_offset, cpy_len);
		for(size_t i = 0; i < cpy_len; i++) {
			pipe->DataPool[(pipe->WriteLocation + i) % pipe->Size] = data[data_offset + i];
		}

		len -= cpy_len;
		data_offset += cpy_len;
		pipe->WriteLocation = (pipe->WriteLocation + cpy_len) % pipe->Size;
		write_len += cpy_len;
		AtomicSubtractSize((size_t*)&pipe->FreeSize, cpy_len);

	}

	AtomicDecrement32((uint32_t*)&pipe->ReferenceCount);

	UnlockSpinlock(pipe->WriteLock);
	return write_len;
}


size_t
ReadPipe(UID id,
		 char *data,
		 size_t len) {
	PipeContext *pipe = (PipeContext*)id;
	LockScheduledSpinlock(pipe->ReadLock);

	AtomicIncrement32((uint32_t*)&pipe->ReferenceCount);

	if(pipe->FreeSize == pipe->Size)
		return UnlockSpinlock(pipe->ReadLock), 0;

	size_t read_len = len;
	size_t max_read_len = pipe->Size - pipe->FreeSize;
	
	if(read_len > max_read_len)
		read_len = max_read_len;

	for(size_t i = 0; i < read_len; i++)
	{
		data[i] = pipe->DataPool[(pipe->ReadLocation + i) % pipe->Size];
	}
	pipe->ReadLocation = (pipe->ReadLocation + read_len) % pipe->Size;
	AtomicAddSize((size_t*)&pipe->FreeSize, read_len);

	AtomicDecrement32((uint32_t*)&pipe->ReferenceCount);

	UnlockSpinlock(pipe->ReadLock);

	return read_len;
}

void
DeletePipe(UID id) {

	PipeContext *pipe = (PipeContext*)id;

	while(pipe->ReferenceCount != 0)YieldThread();

	LockSpinlock(pipe->ReadLock);
	LockSpinlock(pipe->WriteLock);

	FreeMapping(pipe->DataPool, pipe->Size);
	FreeSpinlock(pipe->ReadLock);
	FreeSpinlock(pipe->WriteLock);
	kfree(pipe);
}