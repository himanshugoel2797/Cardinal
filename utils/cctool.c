/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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