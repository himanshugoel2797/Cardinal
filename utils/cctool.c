#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _LINUX_
#include <linux/limits.h>
#elif _CARDINAL_
#include <cardinal/limits.h>
#endif

int main(int argc, char *argv[]) { 
	if(argc != 2 || !strcmp(argv[1], "--help")) {
		printf("%s [option]\n--help : \n\tShows this message\n", argv[0]);
		printf("--lib\n\tPrint the system library install path");
		printf("--inc\n\tPrint the system header install path");
	}

	char path[PATH_MAX];

	if(!strcmp(argv[1], "--lib")) {
		printf(realpath("../lib", path)); 
	}
	
	if(!strcmp(argv[1], "--inc")) { 
		printf(realpath("../include", path));
	}

	return 0;
}