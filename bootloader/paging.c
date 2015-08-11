#include <efi.h>
#include <efilib.h>

#include "memorytypes.h"
#include "paging_struct.h"
#include "memory.h"

void * SetPagingStructs()
{
  EFI_PHYSICAL_ADDRESS pages;

  // 1024 pages for 2
  4MB
  EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, 1024, &pages);
  if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Paging space allocation failed");
    return NULL;
  }

  // 1024 pages at 4kb each
  bootloader_memset(pages, 1024*4*1024);

  // set base CR3 value
  // start setting paging at the next 4kb aligment

  return pages;
}
