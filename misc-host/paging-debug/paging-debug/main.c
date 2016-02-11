#include <stdio.h>
#include "paging.h"

int main(void)
{
  printf("Hello World!\n");


  initCR3();

  uint64_t addr = 4; // GB
  addr *= 1024; // MB
  addr *= 1024; // KB

  for(uint64_t i = 0; i < addr; i += 0x1000)
  {
    SetVirtualAddress(i, i);
  }
  //SetVirtualAddress(0x4000, 0x4000);

  //SetVirtualAddress(0x6523000, 0x6523000);

  return 0;
}

