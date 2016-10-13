TARGET=sys_init.elf

OBJS=main.o initrd.o

LIBS=

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc

CFLAGS=-Iinclude
LDFLAGS=-nostartfiles

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