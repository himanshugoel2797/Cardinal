#include "server.h"
#include <cardinal/process.h>

int main() {

    //TODO Register to the program loader target

    CREATE_NEW_MESSAGE_PTR(request);

    //Poll for a load request
    while(1) {

        while(!GetIPCMessage(request));

        ElfLoadRequest *elf_load_req = (ElfLoadRequest*)request;

        switch(elf_load_req->MsgType) {
        case ElfLoaderMessageType_LoadRequest:
            //Check the permissions of the requesting process and perform the load
            //Once done loading, send the appropriate response.
            break;
        }
    }
}