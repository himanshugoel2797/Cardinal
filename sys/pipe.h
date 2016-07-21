#ifndef _CARDINAL_SYS_IPC_PIPE_H_
#define _CARDINAL_SYS_IPC_PIPE_H_

#include "types.h"

UID
CreatePipe(void);

size_t
WritePipe(UID id,
          const char *data,
          size_t len);

size_t
ReadPipe(UID id,
         char *data,
         size_t len);

void
DeletePipe(UID id);

#endif