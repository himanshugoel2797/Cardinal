TARGET=libcardinalcore.a

CC=x86_64-elf-cardinal-gcc
AR=x86_64-elf-cardinal-ar

HEADER_INSTALL_DIR=cardinal

SOURCES=$(wildcard src/*.c) $(wildcard src/*/*.c)

OBJS=$(SOURCES:.c=.o)

CFLAGS=-std=c11 -Iinclude -I. -Isrc

all:$(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) $(shell cctool --lib)
	mkdir -p $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	cp -r include/. $(shell cctool --inc)/$(HEADER_INSTALL_DIR)

uninstall:
	rm -rf $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	rm -f $(shell cctool --lib)/$(TARGET)