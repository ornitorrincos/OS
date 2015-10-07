#include "bootloader_compat.h"
#include <stdio.h>
#include <stdlib.h>

UEFIPrint Print = wprintf;

void * EFIAPI LoadFile(char * name, UINTN memtype)
{
  FILE * f = fopen(name, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char * mem = (char*)malloc(fsize);
  fread(mem, fsize, 1, f);
  fclose(f);

  return mem;
}
