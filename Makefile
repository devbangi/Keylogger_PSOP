CC=gcc
LOCAL_CFLAGS=-Wall -Werror

obj-m += keylogger.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

load:
	sudo insmod keylogger.ko 

unload:
	sudo rmmod keylogger

clear:
	sudo dmesg -c

view:
	sudo dmesg