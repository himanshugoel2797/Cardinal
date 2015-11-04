CC=clang

include Sources.inc

override INCLUDES += $(CUR_DIR)
override DEFINES+=

SOURCES=$(TARGET_SOURCES)


CURRENT_YEAR=$(shell date +"%Y")

MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
GCC=clang -target x86_64-none-elf
#GCC=x86_64-elf-gcc

CFLAGS= -std=c99 -ffreestanding -Werror -Wall -Wno-unused-function -Wextra -Wno-trigraphs -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O0 -mno-red-zone -mcmodel=kernel

ASM=x86_64-elf-as

.c.o:
	$(GCC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(ASM) $(?:.c=.s) -c -o $(?:.c=.o)
	#rm -f $(?:.c=.s)

.s.o:
	$(ASM) $? -c -o $(?:.s=.o)

.S.o:
	$(GCC) $(addprefix -D, $(DEFINES)) $? -c -o $(?:.S=.o)

.PHONY:debug

debug: build build-tests

# build
all:$(SOURCES)

# clean
clean:
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.s)
