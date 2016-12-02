#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <libipc/ipc.h>
#include <string.h>

#include "server.h"
#include "elf.h"

static UID elf_loader_pid = 0;

ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID pid,
        ElfInformation *elfData
       ) {

    if(elfData == NULL)
        return -1;

    uint32_t key = 0;
    int err = RequestLoadElf(loc, size, limits, pid, &key);
    if(err != 0)
        return err;

    ElfLoaderError error = 0;

    while(!IsElfLoaded(key, &error, elfData));

    return error;
}

int
RequestLoadElf(void *loc,
               uint64_t size,
               ElfLimitations limits,
               UID pid,
               uint32_t *key) {

    if(key == NULL)
        return -1;

    ElfLoadRequest load_req;
    load_req.m.MsgID = RequestMessageID();
    load_req.m.MsgType = CardinalMsgType_Request;
    load_req.MsgType = ElfLoaderMessageType_LoadRequest;
    load_req.Limits = limits;
    load_req.Source = loc;
    load_req.Size = size;
    load_req.TargetPID = pid;

    Message *msg_p = (Message*)&load_req;

    int err = PostIPCMessages(elf_loader_pid, &msg_p, 1);

    if(err != 1)
        return err;

    *key = load_req.m.MsgID;
    return 0;
}

int
IsElfLoaded(uint32_t id,
            ElfLoaderError *error,
            ElfInformation *elfData) {

    if(elfData == NULL | error == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR(msg_buf);
    ElfLoadResponse *load_resp = (ElfLoadResponse*)msg_buf;

    int err = GetIPCMessageFrom((Message*)load_resp, elf_loader_pid, id);

    if(err == 1) {
        *error = load_resp->result;
        memcpy(elfData, &load_resp->info, sizeof(ElfInformation));
    }

    return (err == 1);
}

//TODO cache elf_loader_pid after obtaining it from the service lookup.
//TODO async calls: main entry point actually