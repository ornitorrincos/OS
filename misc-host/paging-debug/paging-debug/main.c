#include <stdio.h>
#include "paging.h"

int main(void)
{
  printf("Hello World!\n");


  initCR3();
  SetVirtualAddress(0x0, 0x0);

  return 0;
}

