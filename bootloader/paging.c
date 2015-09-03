#include <efi.h>
#include <efilib.h>
#include <stdint.h>

#include "memorytypes.h"
#include "paging_struct.h"
#include "memory.h"

uint64_t size;
uint64_t baseaddr;
uint64_t current;

uint64_t count;

EFI_PHYSICAL_ADDRESS pages;
uint64_t * CR3;

uint64_t virtualmemory;
uint64_t maxneg;
//uint64_t base_memory;

uint64_t GetNextEntry()
{
  uint64_t ret = current + 0x1000;//1024*4;

  count++;

  if(count > 1023)
  {
    // need to allocate new page
    // 1024 pages for 4MB
    EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
    if(allocstatus != EFI_SUCCESS)
    {
      Print(L"Paging space allocation failed");
      return -1;
    }
    bootloader_memset((void*)pages, size*0x1000, 0x0);
    baseaddr = (uint64_t)pages;
    ret = baseaddr;

    //Print(L"New Chunk: 0x%llX\n", baseaddr);
    count = 0;

    current = ret;

    if(ret % 0x1000 != 0)
    {
      Print("WARNING Memory not aligned\n");
    }

    return ret;
  }

  //Print(L"New Entry: 0x%llX\n", ret);
  current = ret;

  if(ret % 0x1000 != 0)
  {
    Print("WARNING Memory not aligned\n");
  }

  return ret;
}

uint8_t NeedAllocation(uint64_t in)
{
  if(in == -1)
  {
    return 1;
  }

  return 0;
}

UINT64 EFIAPI GetVMCPUID()
{
  long out = 0;
  long id = 0x80000008;

  __asm__("movq %1, %%rax;"
          "cpuid;"
          "movq %%rax, %0;"
          :"=r"(out)
          :"r"(id)
          );

  return out;
}

uint64_t powerTwo(uint64_t power)
{
  uint64_t ret = 1;

  while(power > 0)
  {
    ret *= 2;

    power--;
  }

  return ret;
}

void initCR3()
{
  size = 1024;

  Print(L"Started CR3\n");

  // 1024 pages for 4MB
  EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
  if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Paging space allocation failed");
    return;
  }

  count = 1;

  // 1024 pages at 4kb each
  bootloader_memset((void*)pages, size*4*1024, 0x0);

  baseaddr = (uint64_t)pages;
  current = baseaddr;

  CR3 = (uint64_t*)current;
  *CR3 = 0;
  ((s_CR3*)CR3)->PCD = 1;
  ((s_CR3*)CR3)->PWT = 1;
  //((s_CR3*)CR3)->base_addr = GetNextEntry();
  UINT64 cpu = GetVMCPUID();

  CPUIDsizes * sizes = (CPUIDsizes*)&cpu;

  Print(L"raw: %x\n", cpu);
  Print(L"Physical: %d\n", sizes->PhysicalAddress);
  Print(L"Virtual: %d\n", sizes->VirtualAddress);
  virtualmemory = sizes->PhysicalAddress;

  int32_t pml4es = sizeof(s_PML4E);

  maxneg = powerTwo(virtualmemory) - 1;
  ((s_CR3*)CR3)->base_addr = maxneg;

}

void printCR3()
{
  Print(L"CR3 value: 0x%llX\n", *((uint64_t*)CR3));
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

  //Print(L"PML4 0x%d\n", tmp->PML4);
  /*Print(L"PDP  %d\n", tmp->PDP);
  Print(L"PD   %d\n", tmp->PD);
  Print(L"PT   %d\n", tmp->PT);*/



  //Print(L"CR3 value: 0x%llX\n", *((uint64_t*)CR3));
  //Print(L"PML4  0x%llX\n", (uint64_t)((s_CR3*)CR3)->base_addr);
  // won't actually work
  if((uint64_t)pml4e == maxneg)
  {
    //Print(L"negative works\n");
    // allocate page
    //Print(L"New PML4\n");
    //Print(L"CR3 value: 0x%llX\n", *((uint64_t*)CR3));
    ((s_CR3*)CR3)->base_addr = GetNextEntry();
    //Print(L"CR3 value: 0x%llX\n", *((uint64_t*)CR3));
    // get the correct entry
    pml4e = ((s_PML4E*)(((s_CR3*)CR3)->base_addr + 8*tmp->PML4));

    for(int i = 0; i < 512; ++i)
    {
      ((s_PML4E*)(((s_CR3*)CR3)->base_addr + 8*i))->PDPBA = maxneg;
    }

    //bootloader_memset(pml4e, 8, 0x0);
    pml4e->PDPBA =  maxneg;
  }

  pml4e = ((s_PML4E*)(((s_CR3*)CR3)->base_addr + 8*tmp->PML4));


  //if(pml4e->P == 0)
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
  if((uint64_t)pdpe == maxneg)
  {
    //Print(L"New PDP\n");
    ((s_PML4E*)pml4e)->PDPBA = GetNextEntry();
    pdpe = ((s_PDPE*)(pml4e->PDPBA + 8*tmp->PDP));

    for(int i = 0; i < 512; ++i)
    {
      ((s_PDPE*)(pml4e->PDPBA + 8*i))->PDBA = maxneg;
    }

    pdpe->PDBA =  maxneg;
  }

  pdpe = (s_PDPE*)(pml4e->PDPBA + 8*tmp->PDP);


  //if(pdpe->P == 0)
  {
    pdpe->P = 1;
    pdpe->RW = 1;
    pdpe->US = 1;
    pdpe->PWT = 1;
    pdpe->PCD = 1;
    pdpe->PS = 0;
    pdpe->NX = 0;
  }

  s_PDE * pde = ((s_PDE*)(pdpe->PDBA));
  if((uint64_t)pde == maxneg)
  {
    //Print(L"New PD\n");
    ((s_PDPE*)pdpe)->PDBA = GetNextEntry();
    pde = (s_PDE*)(pdpe->PDBA + 8*tmp->PD);

    // need to initialize PTBA to maxneg for all of the 512 entries

    for(int i = 0; i < 512; ++i)
    {
      ((s_PDE*)(pdpe->PDBA + 8*i))->PTBA = maxneg;
    }
  }
  pde = (s_PDE*)(pdpe->PDBA + 8*tmp->PD);

  //if(pde->P == 0)
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

  if((uint64_t)pte == maxneg)
  {
    //Print(L"New PT\n");
    pde->PTBA = GetNextEntry();
  }
  pte = (s_PTE*)(pde->PTBA + 8*tmp->PT);

  //if(pte->P == 0)
  {
    pte->P = 1;
    pte->RW = 1;
    pte->US = 1;
    pte->PWT = 1;
    pte->PCD = 1;
    pte->G = 0;
    pte->NX = 0;
    pte->PPBA = phy; // not actually phy directly, but the offset into the 4KB page
  }

  /*Print(L"CR3 value: 0x%llX\n", *(uint64_t*)(((s_CR3*)CR3)->base_addr));
  Print(L"PML4 value: 0x%llX\n", *(uint64_t*)(((s_PML4E*)pml4e)->PDPBA));
  Print(L"PDP value: 0x%llX\n", *(uint64_t*)(((s_PDPE*)pdpe)->PDBA));
  Print(L"PD value: 0x%llX\n", *(uint64_t*)(((s_PDE*)pde)->PTBA));
  Print(L"PT value: 0x%llX\n", *(uint64_t*)(((s_PTE*)pte)->PPBA));*/
}

uint8_t checkIdentity(uint64_t phy)
{
  Print(L"WTF\n");
  s_VPTR * virt = &phy;
  //uint8_t phyvalue = (uint8_t)(*(uint64_t*)phy);
  uint8_t phyvalue = phy;
  uint8_t virtvalue;
  Print(L"WTF\n");
  s_PML4E * pml4e = ((s_PML4E*)(((s_CR3*)CR3)->base_addr + 8*virt->PML4));
  Print(L"WTF\n");
  s_PDPE * pdpe = ((s_PDPE*)(pml4e->PDPBA + 8*virt->PDP));
  Print(L"WTF\n");
  s_PDE * pde = ((s_PDE*)(pdpe->PDBA + 8*virt->PD));
  Print(L"WTF\n");
  s_PTE * pte = ((s_PTE*)(pde->PTBA + 8*virt->PT));
  Print(L"WTF\n");
  virtvalue = *(uint8_t*)(pte->PPBA + virt->offset);
  Print(L"WTF\n");

  // get the correct virtual value

  Print(L"PML4 %d\n", virt->PML4);
  Print(L"PDP  %d\n", virt->PDP);
  Print(L"PD   %d\n", virt->PD);
  Print(L"PT   %d\n", virt->PT);


  Print(L"PHYSICAL: 0x%x\n", phyvalue);
  Print(L"VIRTUAL: 0x%x\n", virtvalue);

  if(phyvalue != virtvalue)
  {
    Print(L"Incorrect physical virtual value\n");
    return 0;
  }

  Print(L"Correct physical virtual value\n");
  return 1;
}
