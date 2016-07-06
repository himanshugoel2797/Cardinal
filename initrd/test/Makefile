TARGET=test.elf
SOURCES=main.o

CC=musl-gcc -static 
CFLAGS=-std=c99 -ffreestanding -Werror -Wall -Wno-unused-parameter -Wno-unused-function -Wextra -Wno-trigraphs $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -O1 -mno-red-zone -mcmodel=kernel -mno-aes -mno-mmx -mno-pclmul -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4a -mno-fma4 -mno-ssse3
AS=x86_64-elf-as


.c.o:
	$(CC) $(CFLAGS) -S $? -o $(?:.c=.s)
	$(AS) $(?:.c=.s) -c -o $(?:.c=.o)
	rm -f $(?:.c=.s)

all:$(SOURCES)
	musl-gcc -static -o $(TARGET) -O2 -mno-red-zone -z max-page-size=0x1000 $(SOURCES)

clean:
	rm -rf $(SOURCES)