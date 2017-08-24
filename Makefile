ifneq ($(KERNELRELEASE),)

	#main object file, which will be used, to create target .ko file. 
	#(e.g. if obj-m = my.o -> my.ko)
	obj-m = main.o
	#other objects, that will be linked into kernel module
	module-objs = 
else

ifeq ($(KPATH),)

KPATH=/lib/modules/`uname -r`/build
endif

.PHONY = default 

default:
	#make command to compile external kernel module
	#-C - path to kernel build environment, 
	#M=... - specifies absolute path to external module resources
	make -C ${KPATH} M=${PWD}

endif
