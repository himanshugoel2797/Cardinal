include Archs.inc

BUILD_DIR=$(HOME)/Documents/Cardinal
INCLUDES=. common managers drivers kmalloc target/hal
DEFINES=MULTIBOOT2

TARGET_ARCH=$(ARCH_x86_64)



OUTDISK=sdb



BOOT_FS=EXT2

CURRENT_YEAR=$(shell date +"%Y")


COM_ENABLED=1
install:COM_ENABLED=0

CONF=DEBUG

MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
CC=clang -target $(TARGET_TRIPLET)
ASFLAGS =-D$(CONF)

CFLAGS= -ffreestanding -Wall -Wextra -Werror -Wno-trigraphs -D$(TARGET_ARCH) -D$(CONF) -DBOOT_FS=$(BOOT_FS)  -DCURRENT_YEAR=$(CURRENT_YEAR) -DCOM_ENABLED=$(COM_ENABLED) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -mno-red-zone -mcmodel=kernel -O0 -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3 -MMD -MP

include $(TARGET_DIR)/$(TARGET_ARCH)/archDefs.inc
include Sources.inc

-include $(SOURCES:.o=.d)

.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(AS) $(ASFLAGS) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

.s.o:
	$(AS) $(ASFLAGS) $? -c -o $(?:.s=.o)

.S.o:
	$(CC) $(ASFLAGS) $? -c -o $(?:.S=.o)

.PHONY:build

run: all

debug: build build-tests

clean_output:
	rm -rf build/$(CONF)/*
	rm -rf ISO/*

# build
build:$(SOURCES) clean_output
	cd $(TARGET_DIR)/$(TARGET_ARCH) && $(MAKE) $(TARGET_MAKE)
	mkdir -p build/$(CONF)
	$(TARGET_ARCH_CC) -T $(TARGET_DIR)/$(TARGET_ARCH)/$(TARGET_LDSCRIPT) -o "build/$(CONF)/kernel.bin" -ffreestanding -O2 -mno-red-zone -nostdlib -z max-page-size=0x1000 $(addprefix $(TARGET_DIR)/$(TARGET_ARCH)/, $(TARGET_SOURCES)) $(SOURCES) -mcmodel=kernel
	cp test build/$(CONF)/initrd
# clean
clean:
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
