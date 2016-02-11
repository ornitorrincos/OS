#include "paging_struct.h"
#include "memory.h"

#include <stdlib.h>
#include <intrin.h>
#include <malloc.h>

uint64_t size;
uint64_t baseaddr;
uint64_t current;

uint64_t count;

EFI_PHYSICAL_ADDRESS pages;
uint64_t * CR3;

uint64_t virtualmemory;
uint64_t maxneg;
uint64_t maxnegcomp;
//uint64_t base_memory;

void bootloader_memset(void* ptr, uint64_t size, uint8_t value)
{
  memset(ptr, value, size);
}

uint64_t GetNextEntry()
{
  uint64_t ret = current + 0x1000;//1024*4;

  count++;

  if(count > 1023)
  {
    // need to allocate new page
    // 1024 pages for 4MB

    //pages = (EFI_PHYSICAL_ADDRESS)malloc(size*0x1000);
  pages = (EFI_PHYSICAL_ADDRESS)_aligned_malloc(size * 0x1000, 0x1000);

    /*EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
    if(allocstatus != EFI_SUCCESS)
    {
      Print(L"Paging space allocation failed");
      return -1;
    }*/
    bootloader_memset((void*)pages, size*0x1000, 0x0);
    baseaddr = (uint64_t)pages;
    ret = baseaddr;

    //Print(L"New Chunk: 0x%llX\n", baseaddr);
    count = 0;

    current = ret;

    if(ret % 0x1000 != 0)
    {
      Print(L"WARNING Memory not aligned\n");
    }

    return ret;
  }

  //Print(L"New Entry: 0x%llX\n", ret);
  current = ret;

  if(ret % 0x1000 != 0)
  {
    Print(L"WARNING Memory not aligned\n");
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

  int arr[4] = {0};

  __cpuid(arr, id);

  out = arr[0];

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
  UINT64 cpu = GetVMCPUID();

  CPUIDsizes * sizes = (CPUIDsizes*)&cpu;

  Print(L"raw: %x\n", cpu);
  Print(L"Physical: %d\n", sizes->PhysicalAddress);
  Print(L"Virtual: %d\n", sizes->VirtualAddress);
  virtualmemory = sizes->PhysicalAddress;

  maxneg = powerTwo(virtualmemory) - 1;
  maxnegcomp = maxneg << PE_ADDR_SHIFT;

  size = 1024;

  Print(L"Started CR3\n");

  // 1024 pages for 4MB
  //EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, MEM_PAGING, size, &pages);
  pages = (EFI_PHYSICAL_ADDRESS)_aligned_malloc(size * 0x1000, 0x1000);
  /*if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Paging space allocation failed");
    return;
  }*/

  count = 1;

  // 1024 pages at 4kb each
  bootloader_memset((void*)pages, size*4*1024, 0x0);

  baseaddr = (uint64_t)pages;
  current = baseaddr;

  CR3 = (uint64_t*)current;
  *CR3 = 0;

  *CR3 |= CR3_PCD;
  *CR3 |= CR3_PWT;
  uint64_t tmpaddr = GetNextEntry();

  memset((uint64_t*)tmpaddr, 0, 0x1000);

  *CR3 &= CR3_ERASE;
  //*CR3 |= CR3SetAddr(tmpaddr);
  *CR3 |= MaskPhyAddr(tmpaddr);

  //*CR3 |=(maxneg << CR3_ADDR_SHIFT);

  //((s_CR3*)CR3)->PCD = 1;
  //((s_CR3*)CR3)->PWT = 1;
  //((s_CR3*)CR3)->base_addr = GetNextEntry();


  //int32_t pml4es = sizeof(s_PML4E);


  //((s_CR3*)CR3)->base_addr = maxneg;

}

void printCR3()
{
  Print(L"CR3 value: 0x%llX\n", *((uint64_t*)CR3));
}

void writeCR3()
{

}

void SetVirtualAddress(uint64_t phy, uint64_t virt)
{
  uint64_t* pml4 = (uint64_t*)MaskPhyAddr(*CR3);
  
  // Get the offsets to each page table
  uint64_t pml4offset = GetPML4Offset(virt);
  uint64_t pdpoffset = GetPDPOffset(virt);
  uint64_t pdoffset = GetPDOffset(virt);
  uint64_t ptoffset = GetPTOffset(virt);


  uint64_t pml4e = pml4[pml4offset];
  
  // if not present, allocate the page
  if((pml4e & PE_P) == 0x0ull)
  {
    //assign the physical addr, and clear all to zero
    uint64_t tmpaddr = GetNextEntry();
    pml4e |= MaskPhyAddr(tmpaddr);

    // initialize the page
    pml4e |= PE_P;
    pml4e |= PE_RW;
    pml4e |= PE_US;
    pml4e |= PE_PWT;
    pml4e |= PE_PCD;
  }
  // assign the values to the array
  pml4[pml4offset] = pml4e;


  uint64_t pdpe = ((uint64_t*)MaskPhyAddr(pml4e))[pdpoffset];
  if((pdpe & PE_P) == 0x0ull)
  {
    // page not present
    uint64_t tmpaddr = GetNextEntry();
    pdpe |= MaskPhyAddr(tmpaddr);

    pdpe |= PE_P;
    pdpe |= PE_RW;
    pdpe |= PE_US;
    pdpe |= PE_PWT;
    pdpe |= PE_PCD;
  }
  ((uint64_t*)MaskPhyAddr(pml4e))[pdpoffset] = pdpe;

  uint64_t pde = ((uint64_t*)MaskPhyAddr(pdpe))[pdoffset];

  if((pde & PE_P) == 0x0ull)
  {
    uint64_t tmpaddr = GetNextEntry();

    pde |= MaskPhyAddr(tmpaddr);

    pde |= PE_P;
    pde |= PE_RW;
    pde |= PE_US;
    pde |= PE_PWT;
    pde |= PE_PCD;
  }
  ((uint64_t*)MaskPhyAddr(pdpe))[pdoffset] = pde;

  uint64_t pte = ((uint64_t*)MaskPhyAddr(pde))[ptoffset];

  // need to make the pt entry

  if((pte & PE_P) == 0x0ull)
  {
    pte |= PE_P;
    pte |= PE_RW;
    pte |= PE_US;
    pte |= PE_PWT;
    pte |= PE_PCD;
    pte |= MaskPhyAddr(phy);
  }
  ((uint64_t*)MaskPhyAddr(pde))[ptoffset] = pte;
}

uint8_t checkIdentity(uint64_t phy)
{
  //uint8_t phyvalue = (uint8_t)(*(uint64_t*)phy);
  uint8_t phyvalue = (uint8_t)*(uint64_t*)phy; // to check values in memory, not really used in hosted environment
  uint8_t virtvalue;
  uint64_t pml4e = CR3GetAddr(*CR3);// + GetPML4Offset(phy);
  pml4e = *(((uint64_t*)pml4e) + GetPML4Offset(phy));
  uint64_t pdpe = *(((uint64_t*)GetAddr(pml4e)) + GetPDPOffset(phy));
  uint64_t pde  = *(((uint64_t*)GetAddr(pdpe)) + GetPDOffset(phy));
  uint64_t pte  = *(((uint64_t*)GetAddr(pde)) + GetPTOffset(phy));
  uint64_t pteptr = ((GetAddr(pte)) + GetPhyOffset(phy));
  virtvalue = (uint8_t)*(uint64_t*)((GetAddr(pte)) + GetPhyOffset(phy));

  //s_PDPE * pdpe = ((s_PDPE*)(pml4e->PDPBA + 8*virt->PDP));
  //s_PDE * pde = ((s_PDE*)(pdpe->PDBA + 8*virt->PD));
  //s_PTE * pte = ((s_PTE*)(pde->PTBA + 8*virt->PT));
  //virtvalue = *(uint8_t*)(pte->PPBA + virt->offset);

  // get the correct virtual value

  Print(L"ADDR: 0x%llx\n", phy);

  Print(L"PML4E 0x%llx\n", pml4e);
  Print(L"PDPE  0x%llx\n", pdpe);
  Print(L"PDE   0x%llx\n", pde);
  Print(L"PTE   0x%llx\n", pte);
  Print(L"pageentry: 0x%llx\n", pteptr);
  Print(L"PPBA  0x%llx\n", GetAddr(pte));

  Print(L"PML4 %d\n", GetPML4Offset(phy));
  Print(L"PDP  %d\n", GetPDPOffset(phy));
  Print(L"PD   %d\n", GetPDOffset(phy));
  Print(L"PT   %d\n", GetPTOffset(phy));
  Print(L"PHY  0x%llx\n", GetPhyOffset(phy));


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
