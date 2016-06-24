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

CFLAGS= -target x86_64-none-elf -std=c99 -ffreestanding -Werror -Wall -Wno-unused-parameter -Wno-unused-function -Wextra -Wno-trigraphs -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O0 -mno-red-zone -mcmodel=kernel -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -MMD -MP

ASM=x86_64-elf-as

.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(ASM) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

.s.o:
	$(ASM) $? -c -o $(?:.s=.o)

.S.o:
	$(CC) $(addprefix -D, $(DEFINES)) $? -c -o $(?:.S=.o)

debug: build build-tests

# build
all:$(SOURCES)

# clean
clean:
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.s)
	rm -f $(SOURCES:.o=.d)
