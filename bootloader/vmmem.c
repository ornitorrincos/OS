#include "vmmem.h"

//#include "paging_struct.h"


EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map, void * kernel)
{

  // the code for setting up paging should go here

  // edit the memory mapping so that the uefi runtime knows about it

  EFI_MEMORY_DESCRIPTOR * tmp = map;

  UINTN elements = size/descriptorsize;
  UINTN i;
  for(i = 0; i < elements; ++i)
  {
    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
    tmp->VirtualStart = tmp->PhysicalStart;
  }

  return uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);
}

