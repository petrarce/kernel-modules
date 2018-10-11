
ifeq ($(OBJ),)
obj-m=reverce.o
else
obj-m=${OBJ}
endif

ifeq ($(KPATH),)
KPATH=/lib/modules/`uname -r`/build
endif

ifeq ($(INSTALL_MOD_PATH),)
INSTALL_MOD_PATH = .
endif

.PHONY = default 

default:build install

build: 
	#make command to compile external kernel module
	#-C - path to kernel build environment, 
	#M=... - specifies absolute path to external module resources
	make  -C ${KPATH} M=${PWD}
	
install:
	install -m 755 *.ko  "$(INSTALL_MOD_PATH)"

clean:
	make -C $(KPATH) M=$(PWD) clean
	
modules:
	make -C $(KPATH) M=$(PWD) modules
help:
	make -C $(KPATH) M=$(PWD) help