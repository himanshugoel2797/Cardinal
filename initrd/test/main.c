#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *mem = malloc(13);
    strcpy(mem, "Hello World!");
    printf(mem);
    return 0;
}
