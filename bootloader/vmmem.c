#include "vmmem.h"

EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map, void * kernel)
{
  // edit the mapping

  EFI_MEMORY_DESCRIPTOR * tmp = map;

  EFI_PHYSICAL_ADDRESS vkernel = 0x280000000;

  UINTN elements = size/descriptorsize;
  UINTN i;
  for(i = 0; i < elements; ++i)
  {
    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
    tmp->VirtualStart = tmp->PhysicalStart;
  }

  // need to find the page for the kernel and map it
  tmp = map;
  for(i = 0; i < elements; ++i)
  {
    if(tmp->PhysicalStart == (EFI_PHYSICAL_ADDRESS)kernel)
    {
      tmp->VirtualStart = vkernel;
      tmp->Type |= EFI_MEMORY_RUNTIME;
      break;
    }

    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
  }

  // kernel not found
  if(i >= elements)
  {
    return -1;
  }

  /*return */uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);

  return EFI_SUCCESS;
}

