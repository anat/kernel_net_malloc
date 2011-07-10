KDIR	=	/lib/modules/$(shell uname -r)/build

FILE	= netmalloc

obj-m	:=	$(FILE).o

#

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *~
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

load:
	sudo insmod $(FILE).ko

unload:
	sudo rmmod $(FILE).ko
