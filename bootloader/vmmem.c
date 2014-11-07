#include "vmmem.h"

EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map)
{
  // edit the mapping

  EFI_MEMORY_DESCRIPTOR * tmp = map;

  UINTN elements = size/descriptorsize;
  UINTN i;
  for(i = 0; i < elements; ++i)
  {
    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
    tmp->VirtualStart = tmp->PhysicalStart;
  }

  uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);

  return EFI_SUCCESS;
}

