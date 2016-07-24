#include "file.h"
#include "kmalloc.h"
#include "managers.h"
#include "list.h"

#include "sys/pipe.h"

int
OpenFileEntry(FileTreeEntry *entry, FileDescriptorFlags flags, FileDescriptorAccessMode aMode) {
	ProcessInformation pInfo;
	GetProcessInformation(GetCurrentProcessUID(), &pInfo);

	for(uint64_t i = 0; i < List_Length(pInfo.FileDescriptors); i++) {
		FileDescriptor *fde = (FileDescriptor*)List_EntryAt(pInfo.FileDescriptors, i);

		LockScheduledSpinlock(fde->lock);

		if(fde->FileData == NULL) {
			fde->FileData = entry;
			fde->Flags = flags;
			fde->AccessMode = aMode;
			UnlockSpinlock(fde->lock);
			return (int)i;
		}

		UnlockSpinlock(fde->lock);
	}

	FileDescriptor *fd = kmalloc(sizeof(FileDescriptor));
	fd->FileData = entry;
	fd->Flags = flags;
	fd->AccessMode = aMode;
	fd->lock = CreateSpinlock();

	List_AddEntry(pInfo.FileDescriptors, fd);
	return (int)(List_Length(pInfo.FileDescriptors) - 1);
}

int
CreateAnonPipe(FileDescriptorFlags flags, int fd[2]) {
	FileTreeEntry *fte = kmalloc(sizeof(FileTreeEntry));
	if(fte == NULL)return -1;

	fte->Type = FileTreeEntryType_Pipe;
	fte->Identifier = CreatePipe();
	fte->ReadLock = CreateSpinlock();
	fte->WriteLock = CreateSpinlock();

	fte->reference_count = 0;

	fd[0] = OpenFileEntry(fte, flags, FileDescriptorAccessMode_Read);
	fd[1] = OpenFileEntry(fte, flags, FileDescriptorAccessMode_Write);

	return 0;
}