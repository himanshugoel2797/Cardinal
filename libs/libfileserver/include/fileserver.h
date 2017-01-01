/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_FILESERVER_H_
#define _LIB_FILESERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <cardinal/file_server.h>

typedef struct {
    int (*open)(const char *, int, uint64_t, uint8_t*, UID, uint64_t*);
    int (*read)(uint64_t, uint64_t, void*, uint64_t, UID);
    int (*write)(uint64_t, uint64_t, void*, uint64_t, UID);
    void (*close)(uint64_t, UID);
    void (*remove)(uint64_t, UID);
    int (*rename)(uint64_t, const char *, UID);
    void (*sync)(uint64_t, UID);
    int (*get_file_properties)(const char *, FileSystemDirectoryEntry*, UID);
} FileServerHandlers;

int
Server_Start(FileServerHandlers *handlers,
             void (*UnknownMessageHandler)(Message *));

int
Server_SetOpMask(FileSystemOpType mask);

FileSystemOpType
Server_GetOpMask(void);

void
Server_Exit(int exitcode);

#endif