#CC = gcc-4.8.3

ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

OBJS            = main.o misc.o ELF.o disk.o vga.o vmmem.o paging.o memory.o other.o
TARGET          = hello.efi

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = $(EFIINCS) -fno-stack-protector -fpic \
		  -fshort-wchar -mno-red-zone -Wall -ggdb3 -O0
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
		  -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS) 

.PHONY: all debug clean

all: $(TARGET)

hello.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

debug: all
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		-j .debug_info -j .debug_abbrev -j .debug_loc \
		-j .debug_aranges -j .debug_line -j .debug_macinfo -j .debug_str\
		--target=efi-app-$(ARCH) hello.so hello-debug.so

clean:
	rm *.so *.efi *.o
