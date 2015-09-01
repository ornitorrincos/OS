#include "memory.h"


void * EFIAPI AllocatePagesType(IN UINTN size, IN UINTN type)
{

  void * ret = NULL;

  Print(L"About to allocate\n");
  EFI_STATUS status = uefi_call_wrapper(BS->AllocatePool, 3, type, size, &ret);


  if(status == EFI_SUCCESS)
  {
    Print(L"Pool Allocated\n");
    return ret;
  }

  Print(L"Allocation Failed\n");
  return NULL;
}

void bootloader_memset(void * in, uint64_t size, uint8_t value)
{
  uint8_t * tmpin = in;
  for(int i = 0; i < size; i++)
  {
    tmpin[i] = value;
  }
}
