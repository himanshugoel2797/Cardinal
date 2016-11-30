TARGET=mem_server.elf

OBJS=main.o mmap.o

LIBS=

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc
OBJCOPY=x86_64-elf-cardinal-objcopy

CFLAGS=-Iinclude -std=c11
LDFLAGS=-nostartfiles

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

install:
	mkdir -p $(shell cctool --srv)/elf
	cp $(TARGET) $(shell cctool --srv)/elf/server.elf
	cp $(TARGET) $(INSTALL_DIR)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf $(TARGET:.bin=.elf)