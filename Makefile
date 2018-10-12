
ifeq ($(OBJ),)
OBJ=reverce.o
endif

ifeq ($(KPATH),)
KPATH=/lib/modules/`uname -r`/build
endif

ifeq ($(INSTALL_MOD_PATH),)
INSTALL_MOD_PATH = .
endif

obj-m=$(OBJ)

.PHONY = default help install clean build modules

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
	@echo 'make OBJ=<obj_files_for_module> KPATH=<path_to_kernel_sorces> INSTALL_MOD_PATH=<path_where_ko_will_be_installed>'
	@echo '	default': 
	@echo '		KPATH=/lib/modules/`uname -r`/build'
	@echo '		INSTALL_MOD_PATH = .'
	@echo '		obj-m=reverce.o'
