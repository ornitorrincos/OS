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
      tmp->Attribute |= EFI_MEMORY_RUNTIME;
      goto setvm; // to force exiting the loop, our job here is finished
    }

    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
  }
  // if we finish the loop it means that we never found the kernel
  return -1;


  setvm:
  return uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);
}

