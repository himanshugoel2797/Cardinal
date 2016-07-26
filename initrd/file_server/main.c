#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <ipc.h>

int main() {
	Message *m = malloc(sizeof(Message));
    while(1)GetIPCMessage(m);
    return 0;
}
