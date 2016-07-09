CC=clang

include Sources.inc

override INCLUDES += $(CUR_DIR)
override DEFINES+=

SOURCES=$(TARGET_SOURCES)


CURRENT_YEAR=$(shell date +"%Y")

MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
#GCC=x86_64-elf-gcc

#-include $(SOURCES:.o=.d)

CFLAGS= -target x86_64-none-elf -std=c99 -ffreestanding -Wall -Wno-unused-parameter -Wno-unused-function -Wextra -Wno-trigraphs -Werror -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O2 -mno-red-zone -mcmodel=kernel -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -emit-llvm

ASM=x86_64-elf-as

%.bc:%.c
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.bc)

%.bc:%.S
	touch $(?:.S=.bc)
	$(ASM) $? -c -o $?.o

.s.o:
	$(ASM) $? -c -o $(?:.s=.o)

.S.o:
	$(CC) $(addprefix -D, $(DEFINES)) $? -c -o $(?:.S=.o)

debug: build build-tests

# build
all:$(SOURCES:.o=.bc)

# clean
clean:
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.s)
	rm -f $(SOURCES:.o=.d)
	rm -f $(SOURCES:.o=.bc)
