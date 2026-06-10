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

# Pre-existing warning classes surfaced by modern clang. Kept as warnings (not
# errors) so -Werror still catches new regressions; remove these as the
# underlying issues are fixed in follow-up PRs.
WARNINGS_TEMP_RELAX= -Wno-error=unused-but-set-variable -Wno-error=unused-but-set-parameter -Wno-error=unused-parameter -Wno-error=return-type -Wno-error=pointer-to-int-cast -Wno-error=unused-function -Wno-error=frame-address -Wno-error=bool-operation

CFLAGS= -target x86_64-none-elf -nostdinc -isystem$(BUILD_DIR)/../libs/libc/include -std=c11 -ffreestanding -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wextra -Wno-trigraphs -Werror $(WARNINGS_TEMP_RELAX) -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) -D_KERNEL_ $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O0 -mno-red-zone -mcmodel=kernel -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -emit-llvm


# Assemble .S/.s via clang targeting bare-metal x86_64 (no GNU cross binutils
# required). Override on the command line to use x86_64-elf-as instead.
ASM ?= clang -target x86_64-none-elf

%.bc:%.c
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.bc)

# NOTE: .S sources are preprocessed, so the -D defines (e.g. MULTIBOOT2, which
# selects the multiboot header in boot.S) must be passed to the assembler.
%.bc:%.S
	touch $(?:.S=.bc)
	$(ASM) $(addprefix -D, $(DEFINES)) $? -c -o $?.o

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
