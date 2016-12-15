TARGET=libpci.a

CC=x86_64-elf-cardinal-gcc
AR=x86_64-elf-cardinal-ar

HEADER_INSTALL_DIR=pci

OBJS=pci_helpers.o

CFLAGS=-std=c11 -Iinclude -I.

all:$(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) $(shell cctool --lib)
	mkdir -p $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	cp include/*.h $(shell cctool --inc)/$(HEADER_INSTALL_DIR)

uninstall:
	rm -rf $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	rm -f $(shell cctool --lib)/$(TARGET)