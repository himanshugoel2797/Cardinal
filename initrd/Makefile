
INITRD_FILES=

export DEFINES
export INCLUDES

all: $(SUBDIRS)
	tar cvf initrd *.*

$(SUBDIRS):
	cd $@ && $(MAKE) all INCLUDES="$(INCLUDES)"
	cp $@/*.elf .

clean:
	find . -name "*.*" -type f -delete
	rm -f initrd

.PHONY: all $(SUBDIRS)
