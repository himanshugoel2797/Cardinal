#include "file.h"
#include "kmalloc.h"
#include "unicode.h"
#include "managers.h"
#include "common.h"

bool
FileMan_CreateEntry(FileTreeEntryType type, 
					FileTreeEntryFlags flags, 
					UID identifier, 
					UID fs_driver, 
					UID owner_id, 
					uint32_t access_perms, 
					const char *path) {

	if(!IsUTF8(path, -1))
		return FALSE;

	FileTreeEntry *fte = kmalloc(sizeof(FileTreeEntry));

	fte->AccessPermissions = access_perms;
	fte->Type = type;
	fte->FileOwnerUID = owner_id;
	fte->FileSystemDriverPID = fs_driver;
	fte->Identifier = identifier;

	if(flags & FileTreeEntryFlags_Locked)
		fte->LockSourcePID = GetCurrentProcessUID();

	fte->ReadLock = CreateSpinlock();
	fte->WriteLock = CreateSpinlock();

	fte->reference_count = 0;

	//Parse the path to get the filename and the tree

}