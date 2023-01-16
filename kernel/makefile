CC				= x86_64-elf-gcc-9.1.0

.PHONY: all clean

all:
	nasm loader.s -f elf64 -o loader.o
	$(CC) -ffreestanding -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c kernel.c -o kernel.o
	$(CC) -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib kernel.o loader.o -lgcc

clean:
	rm *.o
	rm kernel.bin
