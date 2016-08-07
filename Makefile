include Archs.inc

TARGET_ARCH=$(ARCH_x86_64)

BUILD_DIR=$(HOME)/Documents/Cardinal
export BUILD_DIR

INCLUDES= . common managers drivers kmalloc target/hal libs/libc/include/$(TARGET_ARCH) libs/libCardinal/include target/$(TARGET_ARCH)

DEFINES=MULTIBOOT2 $(TARGET_ARCH)
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

CFLAGS= -target $(TARGET_TRIPLET) -nostdinc -isystem$(BUILD_DIR)/libs/libc/include -std=c11 -ffreestanding -Wall -Wextra -Wno-trigraphs -Werror -D$(TARGET_ARCH) -D$(CONF) -DBOOT_FS=$(BOOT_FS) -D_KERNEL_ -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -mno-red-zone -mcmodel=kernel -O0 -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -emit-llvm

include $(TARGET_DIR)/$(TARGET_ARCH)/archDefs.inc
include Sources.inc


%.bc: %.c
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.bc)

%.bc:%.S
	touch $(?:.S=.bc)
	$(AS) $? -c -o $(?:.S=.o)
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
build:$(SOURCES:.o=.bc) clean_output initrd
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) $(TARGET_MAKE)
	llvm-link $(addprefix $(TARGET_DIR)/$(TARGET_ARCH)/, $(TARGET_SOURCES:.o=.bc)) $(SOURCES:.o=.bc) -o tmp.bc
	opt -std-link-opts -O2 tmp.bc -S -o tmp0.bc
	llc tmp.bc -O=0 -mtriple=$(TARGET_TRIPLET) -mattr=-aes,-mmx,-mmx,-pclmul,-sse,-sse2,-sse3,-sse4.1,-sse4.2,-sse4a,-fma,-ssse3 -code-model=kernel -o tmp.S
	rm tmp0.bc tmp.bc
	$(AS) tmp.S -c -o tmp.o
	mkdir -p build/$(CONF)
	$(TARGET_ARCH_LD) -T $(TARGET_DIR)/$(TARGET_ARCH)/$(TARGET_LDSCRIPT) -o "build/$(CONF)/kernel.bin" -ffreestanding -O2 -mno-red-zone -nostdlib -z max-page-size=0x1000 $(wildcard $(addprefix $(TARGET_DIR)/$(TARGET_ARCH)/, $(addprefix *, $(TARGET_SOURCES:.o=.S.o)))) $(wildcard $(addprefix *, $(SOURCES:.o=.S.o))) tmp.o -mcmodel=kernel
	cd initrd && $(MAKE) $(TARGET_MAKE)
	cp initrd/initrd build/$(CONF)/initrd

# clean
clean: clean_initrd
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) clean
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.d)
	rm -f $(SOURCES:.o=.bc)
	rm -rf build/$(CONF)/*
	rm -rf ISO/*

# all
all:build-tests

# build tests
build-tests:build
	mkdir -p ISO
	mkdir -p ISO/isodir
	mkdir -p ISO/isodir/boot
	cp "build/$(CONF)/kernel.bin" ISO/isodir/boot/kernel.bin
	cp "build/$(CONF)/initrd" ISO/isodir/boot/initrd
	mkdir -p ISO/isodir/boot/grub
	cp grub.cfg ISO/isodir/boot/grub/grub.cfg
	grub2-mkrescue -o ISO/os.iso ISO/isodir

# run tests
test: build-tests
# Add your pre 'test' code here...
	qemu-system-x86_64 --enable-kvm -m 4096M -machine q35, -cpu qemu64,+invtsc -smp 4 -d int,cpu_reset,guest_errors -drive id=disk,file=flash.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -net nic,model=rtl8139, -net user -device intel-hda -device hda-duplex -device ich9-usb-uhci3 -device usb-mouse -device usb-kbd -cdrom "ISO/os.iso"

install:clean build-tests
	sudo dd if="ISO/os.iso" of=/dev/$(OUTDISK) && sync
