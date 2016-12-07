TARGET=libccp.a

CC=x86_64-elf-cardinal-gcc
AR=x86_64-elf-cardinal-ar

HEADER_INSTALL_DIR=ccp

OBJS=y.tab.o lex.yy.o ccp.o

CFLAGS=-std=c11 -I../include -I.

y.tab.c:
	yacc -d parser.yacc

lex.yy.c:
	lex analyzer.lex

all:$(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) $(shell cctool --lib)
	mkdir -p $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	cp ../include/*.h $(shell cctool --inc)/$(HEADER_INSTALL_DIR)

uninstall:
	rm -rf $(shell cctool --inc)/$(HEADER_INSTALL_DIR)
	rm -f $(shell cctool --lib)/$(TARGET)