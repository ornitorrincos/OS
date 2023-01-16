.PHONY: all build clean build-bootloader build-kernel clean-bootloader clean-kernel

all: build

build: build-bootloader build-kernel

image:
	cp kernel/kernel.bin hda-image
	cp bootloader/hello.efi hda-image/EFI/Boot/bootx64.efi

run: image
	qemu-system-x86_64 -bios ./OVMF.fd -drive file=fat:rw:hda-image -m 512M

clean: clean-bootloader clean-kernel

build-bootloader:
	$(MAKE) -C bootloader/ MAKEFLAGS=

build-kernel:
	$(MAKE) -C kernel/ MAKEFLAGS=

clean-bootloader:
	$(MAKE) -C bootloader/ MAKEFLAGS= clean

clean-kernel:
	$(MAKE) -C kernel/ MAKEFLAGS= clean
