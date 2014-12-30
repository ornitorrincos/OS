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
