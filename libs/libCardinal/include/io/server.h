/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include "../cardinal_types.h"
#include "../file_server.h"

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
        uint8_t* key,
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

int
IO_GetFileProperties(const char *path,
                     FileSystemDirectoryEntry *info,
                     UID pid);

void
IO_Close(uint64_t, UID);

void
IO_Remove(uint64_t, UID);

int
IO_Rename(uint64_t, const char*, UID);

void
IO_Sync(uint64_t, UID);

#endif