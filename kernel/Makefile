include Archs.inc

TARGET_ARCH=$(ARCH_x86_64)

BUILD_DIR=$(shell pwd)
export BUILD_DIR

INCLUDES= . .. common managers kmalloc target/hal ../libs/libc/include/$(TARGET_ARCH) ../libs/libCardinal/include target/$(TARGET_ARCH)

DEFINES=MULTIBOOT2 $(TARGET_ARCH) _TEST_
export DEFINES


OUTDISK=sdb



BOOT_FS=EXT2
export BOOT_FS

CURRENT_YEAR=$(shell date +"%Y")
export CURRENT_YEAR

COM_ENABLED=1
install:COM_ENABLED=0
export COM_ENABLED


CONF=DEBUG
export CONF

MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
CC=clang
ASFLAGS =-D$(CONF)

# Pre-existing warning classes surfaced by modern clang. Kept as warnings (not
# errors) so -Werror still catches new regressions; remove these as the
# underlying issues are fixed in follow-up PRs.
WARNINGS_TEMP_RELAX= -Wno-error=unused-but-set-variable -Wno-error=unused-but-set-parameter -Wno-error=unused-parameter -Wno-error=return-type -Wno-error=pointer-to-int-cast -Wno-error=unused-function -Wno-error=frame-address -Wno-error=bool-operation

CFLAGS= -g -fsanitize=undefined -fsanitize-trap=undefined -ftrap-function=debug_handle_trap -target $(TARGET_TRIPLET) -nostdinc -isystem ../libs/libc/include -std=c11 -ffreestanding -Wall -Wextra -Wno-unused-variable -Wno-trigraphs -Werror $(WARNINGS_TEMP_RELAX) -D$(TARGET_ARCH) -D$(CONF) -DBOOT_FS=$(BOOT_FS) -DEXEC_ENTRY_POINT=$(EXEC_ENTRY_POINT) -D_KERNEL_ -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -mno-red-zone -mcmodel=kernel -O0 -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -emit-llvm

include $(TARGET_DIR)/$(TARGET_ARCH)/archDefs.inc
include Sources.inc

# Resolve assembly object names at parse time from the committed .S sources.
# A $(wildcard) of the *generated* .S.o files during the build returns stale
# results from make's directory cache (the objects don't exist yet when the
# listing is first cached), which silently dropped boot.S.o -- and with it the
# multiboot header -- from the final link.
TARGET_ASM_OBJS := $(patsubst %.S,%.S.o,$(wildcard $(TARGET_DIR)/$(TARGET_ARCH)/*.S))
# tmp.S is the generated llc intermediate, not a hand-written source; exclude it.
ASM_OBJS := $(patsubst %.S,%.S.o,$(filter-out tmp.S,$(wildcard *.S)))


%.bc: %.c
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.bc)

# .S sources are preprocessed: pass -D defines (e.g. MULTIBOOT2) to the assembler.
%.bc:%.S
	touch $(?:.S=.bc)
	$(AS) $(addprefix -D, $(DEFINES)) $? -c -o $(?:.S=.o)
	cp $(?:.S=.o) $?.o

.s.o:
	$(AS) $(ASFLAGS) $? -c -o $(?:.s=.o)

.S.o:
	$(CC) $(ASFLAGS) $? -c -o $(?:.S=.o)

run: all

debug: build build-tests

clean_output:
	rm -rf build/$(CONF)/*
	rm -rf ISO/*


clean_initrd:
	cd initrd && $(MAKE) clean

# build
build:$(SOURCES:.o=.bc) clean_output
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) $(TARGET_MAKE)
	llvm-link $(addprefix $(TARGET_DIR)/$(TARGET_ARCH)/, $(TARGET_SOURCES:.o=.bc)) $(SOURCES:.o=.bc) -o tmp.bc
	llc tmp.bc -O0 -mtriple=$(TARGET_TRIPLET) -mattr=-aes,-mmx,-pclmul,-sse,-sse2,-sse3,-sse4.1,-sse4.2,-sse4a,-fma,-ssse3 -code-model=kernel -o tmp.S
	rm -f tmp.bc
	$(AS) tmp.S -c -o tmp.o
	mkdir -p build/$(CONF)
	$(TARGET_ARCH_LD) -T $(TARGET_DIR)/$(TARGET_ARCH)/$(TARGET_LDSCRIPT) -o "build/$(CONF)/kernel.bin" -ffreestanding -O2 -mno-red-zone -nostdlib -no-pie -static -z max-page-size=0x1000 $(TARGET_ASM_OBJS) $(ASM_OBJS) tmp.o -mcmodel=kernel
	rm -f tmp.S tmp.o
	cd .. && sh scripts/rebuild-initrd.sh
	cd ../initrd && $(MAKE) $(TARGET_MAKE)
	cp ../initrd/initrd build/$(CONF)/initrd

# clean
clean:
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) clean
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.d)
	rm -f $(SOURCES:.o=.bc)
	rm -rf build/$(CONF)/*
	rm -rf ISO/*

# all
all:build-tests

# grub-mkrescue is named grub-mkrescue on modern distros (was grub2-mkrescue).
GRUB_MKRESCUE ?= grub-mkrescue

# build tests
build-tests:build
	mkdir -p ISO
	mkdir -p ISO/isodir
	mkdir -p ISO/isodir/boot
	cp "build/$(CONF)/kernel.bin" ISO/isodir/boot/kernel.bin
	cp "build/$(CONF)/initrd" ISO/isodir/boot/initrd
	mkdir -p ISO/isodir/boot/grub
	cp grub.cfg ISO/isodir/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o ISO/os.iso ISO/isodir

QEMU ?= qemu-system-x86_64
# Set QEMU_ACCEL=-accel kvm (or pass on the command line) when KVM is available;
# defaults to none so the build also runs under plain TCG / in CI containers.
QEMU_ACCEL ?=
QEMU_MACHINE ?= q35
QEMU_DISK ?= ../disk.img
# Modern device syntax: -soundhw was removed (use -device intel-hda/hda-duplex),
# and the legacy -net nic/-net dump/-net user forms are replaced by -netdev /
# -device / -object filter-dump.
QEMU_FLAGS ?= -m 1024M -machine $(QEMU_MACHINE) $(QEMU_ACCEL) -cpu Haswell,+invtsc,+xsave,+aes -smp 2 \
	-d int,cpu_reset,guest_errors \
	-drive id=disk,file=$(QEMU_DISK),if=none,format=raw -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 \
	-netdev user,id=net0 -device rtl8139,netdev=net0 -object filter-dump,id=dump0,netdev=net0,file=../dump.pcap \
	-device intel-hda -device hda-duplex -device piix3-usb-uhci -device usb-mouse -device usb-kbd \
	-vga vmware -cdrom "ISO/os.iso" -boot d

# run tests (graphical)
test: build-tests
	$(QEMU) $(QEMU_FLAGS)

# run headless: no display, kernel COM1 output to stdio. Useful for CI and for
# the remote PXE/serial mules. Ctrl-A X to exit qemu.
run-headless: build-tests
	$(QEMU) -m 1024M -machine $(QEMU_MACHINE) $(QEMU_ACCEL) -cpu Haswell -smp 2 \
		-d int,cpu_reset,guest_errors -no-reboot -no-shutdown \
		-device piix3-usb-uhci -vga vmware -cdrom "ISO/os.iso" -boot d \
		-display none -serial mon:stdio

install:clean build-tests
	sudo dd if="ISO/os.iso" of=/dev/$(OUTDISK) && sync
