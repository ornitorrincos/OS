#include <efi.h>
#include <efilib.h>
#include <stdint.h>

#include "memorytypes.h"
#include "paging_struct.h"
#include "memory.h"

uint64_t size;
uint64_t baseaddr;
EFI_PHYSICAL_ADDRESS pages;

uint64_t GetNextEntry(uint64_t current)
{
  uint64_t ret = current + 1024*4;

  if((ret - baseaddr) > size*4*1024)
  {
    // need to allocate new page
    // 1024 pages for 4MB
    EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
    if(allocstatus != EFI_SUCCESS)
    {
      Print(L"Paging space allocation failed");
      return -1;
    }
    baseaddr = (uint64_t)pages;
    ret = baseaddr;

    return ret;
  }

  return ret;
}

void * SetPagingStructs()
{


  size = 1024;

  // 1024 pages for 4MB
  EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
  if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Paging space allocation failed");
    return NULL;
  }

  // 1024 pages at 4kb each
  bootloader_memset((void*)pages, size*4*1024);

  // set base CR3 value
  // start setting paging at the next 4kb aligment
  void * base_address = (void*)pages;
  //void * next_address = (void*)(((unsigned char*)pages) + 4*1024);
  void * next_address = (void*)GetNextEntry((uint64_t)base_address);

  ((s_CR3*)base_address)->PWT = 0;
  ((s_CR3*)base_address)->PCD = 0;
  ((s_CR3*)base_address)->base_addr = (uchar)(next_address); // PML4E table

  //base_address = (void*)(((s_CR3*)base_address)->base_addr);
  base_address = next_address;
  next_address = (void*)GetNextEntry((uint64_t)base_address);
  //next_address = (void*)(((unsigned char*)base_address) + 4*1024);

  // PML4E
  ((s_PML4E*)base_address)->P = 1;
  ((s_PML4E*)base_address)->RW = 1;
  ((s_PML4E*)base_address)->US = 1;
  ((s_PML4E*)base_address)->PWT = 1;
  ((s_PML4E*)base_address)->PCD = 1;
  ((s_PML4E*)base_address)->PS = 0;
  //((s_PML4E*)base_address)->G = 0;
  ((s_PML4E*)base_address)->NX = 0;
  ((s_PML4E*)base_address)->PDPBA = (uchar)next_address;

  // PDPTE
  base_address = next_address;
  next_address = (void*)GetNextEntry((uint64_t)base_address);
  ((s_PDPE*)base_address)->P = 1;
  ((s_PDPE*)base_address)->RW = 1;
  ((s_PDPE*)base_address)->US = 1;
  ((s_PDPE*)base_address)->PWT = 1;
  ((s_PDPE*)base_address)->PCD = 1;
  ((s_PDPE*)base_address)->PS = 0;
  ((s_PDPE*)base_address)->NX = 0;
  ((s_PDPE*)base_address)->PDBA = (uchar)next_address;


  // PTE
  // need a for loop here to setup paging
  base_address = next_address;
  next_address = (void*)GetNextEntry((uint64_t)base_address);
  //((s_PTE*))




  return (void*)pages;
}
