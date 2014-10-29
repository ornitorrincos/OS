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

  int * toVwrite = (int*)tmp->VirtualStart;

  (*(EFI_PHYSICAL_ADDRESS*)0x150) = (EFI_PHYSICAL_ADDRESS)towrite;
  (*(EFI_PHYSICAL_ADDRESS*)0x200) = (EFI_PHYSICAL_ADDRESS)toVwrite;
  (*towrite) = (int)0xABCD;
  (*toVwrite) = (int)0xDCBA;
  // we found the location
  tmp->VirtualStart = 0x40000000; // For Some Reason it doesn't get applied, might need to setup every mapping also?
  (*(EFI_PHYSICAL_ADDRESS*)0x200) = size;
  (*(EFI_PHYSICAL_ADDRESS*)0x150) = descriptorsize;
  return uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);
}

