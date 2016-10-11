TARGET=libcserver.a

CC=x86_64-elf-cardinal-gcc
AR=x86_64-elf-cardinal-ar

OBJS=open.o read.o write.o close.o direntry.o mount_db.o interface_provider.o mount.o list.o stat.o remove.o cserver.o fd2path.o 

CFLAGS=-std=c11 -Iinclude -I. -D_MOUNT_DB_PRIV_H_

all:$(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) $(shell cctool --lib)
	mkdir -p $(shell cctool --inc)/cserver
	cp include/*.h $(shell cctool --inc)/cserver