KDIR := /lib/modules/4.9.180-1-MANJARO/build
PWD  := $(shell pwd)

MODULE_NAME := mfrc522

obj-m := mfrc522.o

all:
	make -C "$(KDIR)" SUBDIRS="$(PWD)" modules

install:
	make -C "$(KDIR)" SUBDIRS="$(PWD)" modules_install

clean:
	make -C "$(KDIR)" SUBDIRS="$(PWD)" clean
