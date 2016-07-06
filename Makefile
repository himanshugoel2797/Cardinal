include Archs.inc

TARGET_ARCH=$(ARCH_x86_64)

LIBC_DIR=$(HOME)/Documents/musl-1.1.14

BUILD_DIR=$(HOME)/Documents/Cardinal

INCLUDES=. common managers drivers kmalloc target/hal

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

CFLAGS=  -target $(TARGET_TRIPLET) -ffreestanding -Wall -Wextra -Wno-trigraphs -Werror -D$(TARGET_ARCH) -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -mno-red-zone -mcmodel=kernel -O0 -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3

include $(TARGET_DIR)/$(TARGET_ARCH)/archDefs.inc
include Sources.inc


.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(AS) $(ASFLAGS) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

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
build:$(SOURCES) clean_output initrd
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) $(TARGET_MAKE)
	mkdir -p build/$(CONF)
	$(TARGET_ARCH_CC) -T $(TARGET_DIR)/$(TARGET_ARCH)/$(TARGET_LDSCRIPT) -o "build/$(CONF)/kernel.bin" -ffreestanding -O2 -mno-red-zone -nostdlib -z max-page-size=0x1000 $(addprefix $(TARGET_DIR)/$(TARGET_ARCH)/, $(TARGET_SOURCES)) $(SOURCES) -mcmodel=kernel
	cd initrd && $(MAKE) $(TARGET_MAKE)
	cp initrd/initrd build/$(CONF)/initrd

# clean
clean: clean_initrd
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) clean
	rm -f $(SOURCES)
	rm -f $(SOURCES:.o=.d)
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
	qemu-system-x86_64 -enable-kvm -m 4096M -machine q35 -cpu host -smp 4 -d int,cpu_reset,guest_errors -drive id=disk,file=flash.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -net nic,model=rtl8139, -net user -device intel-hda -device hda-duplex -device ich9-usb-uhci3 -device usb-mouse -device usb-kbd -cdrom "ISO/os.iso"

install:clean build-tests
	sudo dd if="ISO/os.iso" of=/dev/$(OUTDISK) && sync
