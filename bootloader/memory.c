#include "memory.h"

UINTN EFIAPI ceil(IN float x)
{
  UINTN ret = x,;
  if (x - (int)x > 0) {
      x += 1;
      ret = (int)x;
  }

  return ret;
}

void * EFIAPI AllocatePages2(IN UINTN size)
{
  float pages = (size/1024)/4;

  UINTN ipages = ceil(pages);

  void * ret = NULL;

  EFI_PHYSICAL_ADDRESS tmp;

  Print(L"About to allocate\n");
  EFI_STATUS status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, ipages, &tmp);

  Print(L"Pages Allocated\n");
  if(status == EFI_SUCCESS)
  {
    ret = (void)tmp;
    return ret;
  }

  return NULL;
}
