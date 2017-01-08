
INITRD_FILES=

all:
	tar cvf initrd $(INITRD_FILES) *.*

clean:
	find . -name "*.*" -type f -delete
	rm -f initrd

.PHONY: all
