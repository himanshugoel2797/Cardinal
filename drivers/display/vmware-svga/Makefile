TARGET=vmware-svga.elf

OBJS=src/main.o src/fileserver.o src/svga.o

LIBS=-lfileserver -lpci -Wl,--start-group -lcardinalcore -lc -Wl,--end-group

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc
OBJCOPY=x86_64-elf-cardinal-objcopy

CFLAGS=-Iinclude -Ivmware -std=c11
LDFLAGS=

.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
	rm -rf $(TARGET:.bin=.elf)