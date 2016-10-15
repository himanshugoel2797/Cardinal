TARGET=userboot.bin

OBJS=main.o initrd.o program.o elf.o

LIBS=

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc
OBJCOPY=x86_64-elf-cardinal-objcopy

CFLAGS=-Iinclude
LDFLAGS=-nostartfiles

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET:.bin=.elf)
	$(OBJCOPY) -O binary $(TARGET:.bin=.elf) $(TARGET)

install:
	cp *.bin $(INSTALL_DIR)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf $(TARGET:.bin=.elf)