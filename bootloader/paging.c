#include <efi.h>
#include <efilib.h>

#include "memorytypes.h"
#include "paging_struct.h"
#include "memory.h"

void * SetPagingStructs()
{
  EFI_PHYSICAL_ADDRESS pages;

  // 1024 pages for 4MB
  EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, 1024, &pages);
  if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Paging space allocation failed");
    return NULL;
  }

  // 1024 pages at 4kb each
  bootloader_memset((void*)pages, 1024*4*1024);

  // set base CR3 value
  // start setting paging at the next 4kb aligment
  void * base_address = (void*)pages;
  void * next_address = (void*)(((unsigned char*)pages) + 4*1024);

  ((s_CR3*)base_address)->PWT = 0;
  ((s_CR3*)base_address)->PCD = 0;
  ((s_CR3*)base_address)->base_addr = (uchar)(next_address); // PML4E table

  //base_address = (void*)(((s_CR3*)base_address)->base_addr);
  base_address = next_address;
  next_address = (void*)(((unsigned char*)base_address) + 4*1024);

  // PML4E

  // PDPTE

  // PTE




  return (void*)pages;
}
