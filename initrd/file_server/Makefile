TARGET=fileserver.elf
SOURCES=main.o

CC=musl-gcc -static 
CFLAGS=-std=c11 -ffreestanding -Wall -Wno-unused-parameter -Wno-unused-function -Wextra -Wno-trigraphs $(addprefix -D, $(DEFINES)) -O0
AS=x86_64-elf-as


.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(AS) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

all:$(SOURCES)
	musl-gcc -static -o $(TARGET) -O2 $(SOURCES)

clean:
	rm -rf $(SOURCES)