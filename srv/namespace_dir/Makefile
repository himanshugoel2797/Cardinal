TARGET=namespace_dir.elf

OBJS=main.o namespace_dir.o

LIBS=-llist

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc
OBJCOPY=x86_64-elf-cardinal-objcopy

CFLAGS=-Iinclude -std=c11
LDFLAGS=

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

install:
	mkdir -p $(shell cctool --srv)/namespace_dir
	cp $(TARGET) $(shell cctool --srv)/namespace_dir/server.elf
	cp $(TARGET) $(INSTALL_DIR)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf $(TARGET:.bin=.elf)