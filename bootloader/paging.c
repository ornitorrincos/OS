#include <efi.h>
#include <efilib.h>
#include <stdint.h>

#include "memorytypes.h"
#include "paging_struct.h"
#include "memory.h"

uint64_t size;
uint64_t baseaddr;
uint64_t current;

EFI_PHYSICAL_ADDRESS pages;
uint64_t * CR3;

//uint64_t base_memory;

uint64_t GetNextEntry()
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

void initCR3()
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

  baseaddr = (uint64_t)pages;
  current = baseaddr;

  CR3 = current;
  ((s_CR3*)CR3)->PCD = 1;
  ((s_CR3*)CR3)->PWT = 1;
}

void writeCR3()
{
  __asm__("movq %0, %%cr3;"
          :
          :"r"(*CR3));
}

void SetVirtualAddress(uint64_t phy, uint64_t virt)
{
  s_VPTR * tmp = (s_VPTR*)&virt;
  s_PML4E * pml4e = ((s_PML4E*)(((s_CR3*)CR3)->base_addr));

  if((uint64_t)pml4e == 0)
  {
    // allocate page
    (((s_CR3*)CR3)->base_addr) = GetNextEntry();
    // get the correct entry
    pml4e = ((s_PML4E*)(((s_CR3*)CR3)->base_addr + 8*tmp->PML4));
  }

  if(pml4e->P == 0)
  {
    // initialize the page
    pml4e->P = 1;
    pml4e->RW = 1;
    pml4e->US = 1;
    pml4e->PWT = 1;
    pml4e->PCD = 1;
    pml4e->PS = 0;
    pml4e->NX = 0;
  }

  s_PDPE * pdpe = ((s_PDPE*)((s_PML4E*)(pml4e->PDPBA)));
  if((uint64_t)pdpe == 0)
  {
    ((s_PML4E*)pml4e)->PDPBA = GetNextEntry();
    pdpe = ((s_PDPE*)(((s_PML4E*)pml4e->PDPBA) + 8*tmp->PDP));
  }

  if(pdpe->P == 0)
  {
    pdpe->P = 1;
    pdpe->RW = 1;
    pdpe->US = 1;
    pdpe->PWT = 1;
    pdpe->PCD = 1;
    pdpe->PS = 0;
    pdpe->NX = 0;
  }

  s_PDE * pde = ((s_PDE*)((s_PDPE*)pdpe->PDBA));
  if((uint64_t)pde == 0)
  {
    ((s_PDPE*)pdpe)->PDBA = GetNextEntry();
    pde = ((s_PDE*)(((s_PDPE*)pdpe->PDBA) + 8*tmp->PD));
  }

  if(pde->P == 0)
  {
    pde->P = 1;
    pde->RW = 1;
    pde->US = 1;
    pde->PWT = 1;
    pde->PCD = 1;
    //pde->PS = 0;
    pde->NX = 0;
  }

  s_PTE * pte = ((s_PTE*)((s_PDE*)pde->PTBA));
  if((uint64_t)pte == 0)
  {
    ((s_PDE*)pde)->PTBA = GetNextEntry();
    pte = ((s_PTE*)(((s_PDE*)pde->PTBA) + 8*tmp->PT));
  }

  if(pte->P == 0)
  {
    pte->P = 1;
    pte->RW = 1;
    pte->US = 1;
    pte->PWT = 1;
    pte->PCD = 1;
    pte->G = 0;
    pte->NX = 0;
    pte->PPBA = phy;
  }
}
