#include <syscall.h>

int main() {

    int k_io = RegisterDriver("PCI", 0, 0);

    while(1);
    return k_io;
}
