#include "initrd.h"
#include <libio/fileserver.h>

int main() {
    //First map in the initrd
    ImportInitrd();

    //Use it along with the loaded elf loader to load the initial system processes
    uint8_t key[KEY_BYTES];
    uint64_t fd = 0;

    UID pid = 0;
    uint64_t error = 0;

    uint64_t buf_len = 1920 * 1080 * 4;
    uint64_t vAddr = 0;
    uint64_t read_key = 0, write_key = 0;
    IO_AllocateBuffer(&buf_len,
                      &vAddr,
                      &read_key,
                      &write_key);

    memset((void*)vAddr, 0xFF, buf_len);

    uint32_t tmp_key = 0;
    RetrieveNamespace("disp0", &tmp_key);
    while(!IsNamespaceRetrieved(tmp_key, &pid, &error));

    IO_Open(":framebuffer", FileSystemOpFlag_Write, 0, key, pid, &fd);
    IO_Write(fd, 0, write_key, buf_len, pid);

    //Wait until exit requested on power off
    while(1);
}