#include "vmmem.h"

EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map)
{
  // edit the mapping

  // find the 1MB
  EFI_MEMORY_DESCRIPTOR * tmp = map;
  while(tmp->PhysicalStart < (1*1024*1024))
  {
    tmp = (EFI_MEMORY_DESCRIPTOR*)((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize);
  }

  int * towrite = (int*)tmp->PhysicalStart;

  (*(EFI_PHYSICAL_ADDRESS*)0x150) = (EFI_PHYSICAL_ADDRESS)towrite;
  (*towrite) = (int)0xABCD;
  // we found the location
  tmp->VirtualStart = 0x40000000;

  return uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);
}

