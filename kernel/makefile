all:
	x86_64-elf-gcc-4.8.4 -ffreestanding -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c kernel.c -o kernel.o
	x86_64-elf-gcc-4.8.4 -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib kernel.o -lgcc
