CFLAGS=-std=c11

PROGS=true.elf false.elf cctool.elf 

.elf.o:
	$(LD) $^ -o $@

true.elf:true.o

false.elf:false.o

cctool.elf:cctool.o

all:$(PROGS)
