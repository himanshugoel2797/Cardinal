TARGET=test.elf
SOURCES=main.o

CC=clang -target x86_64-none-elf
CFLAGS=-std=c99 -ffreestanding -Werror -Wall -Wno-unused-parameter -Wno-unused-function -Wextra -Wno-trigraphs $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O1 -mno-red-zone -mcmodel=kernel -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3
AS=x86_64-elf-as


.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(AS) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

all:$(SOURCES)
	x86_64-elf-gcc -o $(TARGET) -ffreestanding -O2 -mno-red-zone -nostdlib -z max-page-size=0x1000 $(SOURCES) -lgcc

clean:
	rm -rf $(SOURCES)