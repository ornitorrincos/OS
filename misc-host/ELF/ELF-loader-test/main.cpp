#include <stdio.h>
#include "bootloader_compat.h"
#include "elfinfoprint.h"

int main(void)
{
  void * kernel = LoadFile("kernel.bin", 0);

  ElfInfoPrint((Elf64_Ehdr *)kernel);
  return 0;
}

