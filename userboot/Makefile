TARGET=userboot.bin

OBJS=main.o initrd.o elf.o program.o

LIBS=

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc
OBJCOPY=x86_64-elf-cardinal-objcopy

CFLAGS=-Iinclude -std=c11
LDFLAGS=-nostartfiles -T linker.ld

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET:.bin=.elf)
	$(OBJCOPY) --strip-all -O binary $(TARGET:.bin=.elf) $(TARGET)

install:
	cp *.bin $(INSTALL_DIR)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf $(TARGET:.bin=.elf)