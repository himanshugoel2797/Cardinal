TARGET=libcserver.a

CC=x86_64-elf-cardinal-gcc
AR=x86_64-elf-cardinal-ar

OBJS=open.o read.o write.o close.o direntry.o mount_db.o interface_provider.o link.o mount.o list.o stat.o unlink.o cserver.o

CFLAGS=-std=c11 -Iinclude -I.

all:$(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) $(shell cctool --lib)
	mkdir -p $(shell cctool --inc)/cserver
	cp include/*.h $(shell cctool --inc)/cserver