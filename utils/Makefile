CFLAGS=-std=c11

PROGS=true.elf false.elf cctool.elf 

%.elf:%.o
	$(CC) $^ -o $@

%.exec:%.elf
	mv $^ $(addprefix $(shell cctool --bin)/,$(basename $@))

true.elf:true.o

false.elf:false.o

cctool.elf:cctool.o

all:$(PROGS)

install: $(PROGS:.elf=.exec)

clean:
	rm -rf $(PROGS) $(PROGS:.elf=.o) $(basename $(PROGS))