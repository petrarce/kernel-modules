obj-m = main.o
ifndef KPATH
	KPATH=/lib/modules/`uname -r`/build
endif

.PHONY = all clear

all:
	make -C ${KPATH} M=${PWD}

clear:
