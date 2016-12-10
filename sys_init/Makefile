TARGET=sys_init.elf

OBJS=main.o initrd.o png.o

LIBS=-lpng -lz -Wl,--start-group -lcardinalcore -lc -Wl,--end-group

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc

CFLAGS=-Iinclude
LDFLAGS=

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

install:
	cp *.elf $(INSTALL_DIR)
	cp *.data $(INSTALL_DIR)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)