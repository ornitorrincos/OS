#include "vmmem.h"

#include "paging_struct.h"

// might need to cast t into an int64?
// that's the size it has anyways
void EFIAPI SetCR3(CR3 t)
{
  //asm("mov t, %cr3"); // need to look better at inline asm
}

EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map, void * kernel)
{

  // the code for setting up paging should go here

  // edit the memory mapping so that the uefi runtime knows about it

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
    }

    tmp = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)tmp + descriptorsize));
  }

  if(i == elements)
  {
    // if we finish the loop it means that we never found the kernel
    return -1;
  }

  return uefi_call_wrapper(RT->SetVirtualAddressMap, 4, size, descriptorsize, descriptorversion, map);
}

