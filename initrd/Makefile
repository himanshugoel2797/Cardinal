
SUBDIRS := $(wildcard */.)

export DEFINES
export INCLUDES

all: $(SUBDIRS)
	tar cvf initrd *.elf 

$(SUBDIRS):
	cd $@ && $(MAKE) all INCLUDES="$(INCLUDES)"
	cp $@/*.elf .

clean:
	find . -name "*.elf" -type f -delete
	find . -name "*.o" -type f -delete
	rm -f initrd

.PHONY: all $(SUBDIRS)
