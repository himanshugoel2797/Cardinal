#include <signal.h>

void sigHandle(int sig) {
    while(1);
}

int main() {
    signal(SIGINT, sigHandle);
    while(1);
    return 0;
}
