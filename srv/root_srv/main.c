#include <cardinal/ipc.h>
#include <cserver/cserver.h>


int main() {
    return cserver_main(CARDINAL_IPCDEST_FILESERVER, NULL, NULL);
}