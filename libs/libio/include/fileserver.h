#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/file_server.h>

typedef struct {
    uint64_t Size;
} FileInfo;

int
IO_AllocateBuffer(uint64_t* len, 
				  uint64_t* address, 
				  uint64_t* read_key,
				  uint64_t* write_key);

int
IO_FreeBuffer(uint64_t address,
			  uint64_t len,
			  uint64_t read_key,
			  uint64_t write_key);

int 
IO_Open(const char* path, 
		int flags, 
		uint64_t mode, 
		uint64_t* key, 
		UID pid, 
		uint64_t* fd);

int 
IO_Read(uint64_t fd, 
		uint64_t offset,
		uint64_t key, 
		uint64_t len, 
		UID pid);

int 
IO_Write(uint64_t fd, 
		 uint64_t offset, 
		 uint64_t key,
		 uint64_t len, 
		 UID pid);

void 
IO_Close(uint64_t, UID);

void 
IO_Remove(uint64_t, UID);

void 
IO_GetInfo(const char*, UID, FileInfo*);

int 
IO_Rename(uint64_t, const char*, UID);

void 
IO_Sync(uint64_t, UID);

#endif