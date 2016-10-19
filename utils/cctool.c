#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifdef __linux__
#include <linux/limits.h>
#elif __cardinal__
#include <cardinal/limits.h>
#endif

void
showHelp(char *name) {
    printf("%s [option]\n", name);
    printf("--help\n\tShows this message\n");
    printf("--lib\n\tPrint the system library install path\n");
    printf("--inc\n\tPrint the system header install path\n");
    printf("--bin\n\tPrint the system executable install path\n");
}

int main(int argc, char *argv[]) {
    if(argc != 2 || !strcmp(argv[1], "--help")) {
        showHelp(argv[0]);
        return 0;
    }

    char path[PATH_MAX];

    char tmp_path[64];
    sprintf(tmp_path, "/proc/%d/exe", getpid());
    int len = readlink(tmp_path, path, PATH_MAX);

    for(; len >= 0; len--) {
        if(path[len] == '/') {
            path[len] = 0;
            break;
        }
    }

    if(!strcmp(argv[1], "--lib")) {
        strcat(path, "/../lib");
    } else if(!strcmp(argv[1], "--inc")) {
        strcat(path, "/../include");
    } else if(!strcmp(argv[1], "--bin")) {
        strcat(path, "/../bin");
    } else if(!strcmp(argv[1], "--srv")) {
        strcat(path, "/../../srv");
    } else {
        showHelp(argv[0]);
        return 0;
    }

    printf(path);

    return 0;
}