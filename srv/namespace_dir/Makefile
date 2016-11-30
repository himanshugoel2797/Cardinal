TARGET=namespace_dir.elf

OBJS=main.o namespace_dir.o

LIBS=

CC=x86_64-elf-cardinal-gcc
LD=x86_64-elf-cardinal-gcc


.PHONY:all

all:$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LIBS) -o $(TARGET)

install:
	cp $(TARGET) $(shell cctool --bin)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)