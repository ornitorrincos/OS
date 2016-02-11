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

  __asm__ __volatile__ ("movq %1, %%rax;"
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
  UINT64 cpu = GetVMCPUID();

  CPUIDsizes * sizes = (CPUIDsizes*)&cpu;

  Print(L"raw: %x\n", cpu);
  Print(L"Physical: %d\n", sizes->PhysicalAddress);
  Print(L"Virtual: %d\n", sizes->VirtualAddress);
  virtualmemory = sizes->PhysicalAddress;

  maxneg = powerTwo(virtualmemory) - 1;

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

  *CR3 |= CR3_PCD;
  *CR3 |= CR3_PWT;
  *CR3 |=(maxneg << CR3_ADDR_SHIFT);

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
  __asm__ __volatile__("movq %0, %%cr3;"
          :
          :"r"(*CR3));
}

void SetVirtualAddress(uint64_t phy, uint64_t virt)
{
  //uint64_t pml4e = CR3GetAddr(*CR3);
  uint64_t pml4e = MaskPhyAddr(*CR3);

  if(pml4e == 0xFFFFFFF000ull)//maxneg)
  {


    uint64_t tmpaddr = GetNextEntry();

    *CR3 &= CR3_ERASE;
    //*CR3 |= CR3SetAddr(tmpaddr);
    *CR3 |= MaskPhyAddr(tmpaddr);


    pml4e &= PE_ERASE;
    pml4e = tmpaddr;

    Print(L"maxneg: 0x%llX\n", maxneg);

    for(int i = 0; i < 512; ++i)
    {
      *(((uint64_t*)pml4e) + i) = 0;
      *(((uint64_t*)pml4e) + i) |= SetAddr(maxneg);

    }
    Print(L"PDPE entry 1: 0x%llX\n", *(((uint64_t*)pml4e) + 1));
  }

  pml4e = (uint64_t)(((uint64_t*)pml4e) + GetPML4Offset(virt));
  //Print(L"PML4E 0x%llX\n", pml4e);

  if(((*(uint64_t*)pml4e) & PE_P) == 0x0ull)
  {
    // initialize the page
    (*(uint64_t*)pml4e) |= PE_P;
    (*(uint64_t*)pml4e) |= PE_RW;

    if(GetPDPOffset(virt) == 0)
    {
      //Print(L"Addr: 0x%llX\n", virt);
      //Print(L"PDPE offset:: %lld\n", GetPDPOffset(virt));
      //Print(L"PDPE offset:: 0x%llX\n", GetPDPOffset(virt));
      //Print(L"PDPE 1: 0x%llX\n", pdpe);
    }
    (*(uint64_t*)pml4e) |= PE_US;
    (*(uint64_t*)pml4e) |= PE_PWT;
    (*(uint64_t*)pml4e) |= PE_PCD;
  }

  //Print(L"PDPE offset:: 0x%llX\n", GetPDPOffset(virt));

  // need to get this right
  uint64_t pdpe = *((uint64_t*)(MaskPhyAddr(pml4e) + GetPDPOffset(virt)*8));
  // need to reorganize the rest of the code


  if(MaskPhyAddr(pdpe) == 0xFFFFFFF000ull)//maxneg)
  {
    Print(L"PDPE CHECK OK\n");
    uint64_t tmpaddr = GetNextEntry();

    *(uint64_t*)pml4e &= PE_ERASE;
    //*(uint64_t*)pml4e |= SetAddr(tmpaddr);
    *(uint64_t*)pml4e |= MaskPhyAddr(tmpaddr);


    pdpe &= PE_ERASE;
    pdpe = tmpaddr;

    for(int i = 0; i < 512; ++i)
    {
      *(((uint64_t*)pdpe) + i) = 0;
      *(((uint64_t*)pdpe) + i) |= SetAddr(maxneg);
    }
  }


  if(((*(uint64_t*)pdpe) & PE_P) == 0x0ull)
  {
    (*(uint64_t*)pdpe) |= PE_P;
    (*(uint64_t*)pdpe) |= PE_RW;
    (*(uint64_t*)pdpe) |= PE_US;
    (*(uint64_t*)pdpe) |= PE_PWT;
    (*(uint64_t*)pdpe) |= PE_PCD;
  }



  //Print(L"PDPE 0x%llx\n", *(uint64_t*)pdpe);

  //uint64_t pde = GetAddr(*(uint64_t*)pdpe);
  uint64_t pde = *((uint64_t*)(MaskPhyAddr(pdpe) + GetPDOffset(virt)*8));


  //Print(L"PDE 0x%llx\n", pde);

  //Print(L"pml4: %d pdpe: %d pde: %d pte: %d\n", GetPML4Offset(virt), GetPDPOffset(virt), GetPDOffset(virt), GetPTOffset(virt));

  if(MaskPhyAddr(pde) == 0xFFFFFFF000ull)//maxneg)
  {
    Print(L"PDE CHECK OK\n");
    Print(L"pml4: %d pdpe: %d pde: %d pte: %d\n", GetPML4Offset(virt), GetPDPOffset(virt), GetPDOffset(virt), GetPTOffset(virt));

    if(GetPDOffset(virt) != 0)
    {
      while(1){};
    }

    uint64_t tmpaddr = GetNextEntry();

    *(uint64_t*)pdpe &= PE_ERASE;
    //*(uint64_t*)pdpe |= SetAddr(tmpaddr);
    *(uint64_t*)pdpe |= MaskPhyAddr(tmpaddr);


    pde &= PE_ERASE;
    pde = tmpaddr;

    for(int i = 0; i < 512; ++i)
    {
      *(((uint64_t*)pde) + i) = 0;
      *(((uint64_t*)pde) + i) |= SetAddr(maxneg);
    }

  }



  //pde = (uint64_t)(((uint64_t*)pde) + GetPDOffset(virt));

  if(((*(uint64_t*)pde) & PE_P) == 0x0ull)
  {
    (*(uint64_t*)pde) |= PE_P;
    (*(uint64_t*)pde) |= PE_RW;
    (*(uint64_t*)pde) |= PE_US;
    (*(uint64_t*)pde) |= PE_PWT;
    (*(uint64_t*)pde) |= PE_PCD;
  }

  uint64_t pte = *((uint64_t*)(MaskPhyAddr(pde) + GetPTOffset(virt)*8));



  //Print(L"PDPE 0x%llx\n", pdpe);

  if(MaskPhyAddr(pte) == 0xFFFFFFF000ull)//maxneg)
  {
    //Print(L"PTE CHECK OK\n");
    uint64_t tmpaddr = GetNextEntry();

    *(uint64_t*)pde &= PE_ERASE;
    //*(uint64_t*)pde |= SetAddr(tmpaddr);
    *(uint64_t*)pde |= MaskPhyAddr(tmpaddr);


    pte &= PE_ERASE;
    pte = tmpaddr;

    for(int i = 0; i < 512; ++i)
    {
      *(((uint64_t*)pte) + i) = 0;
      *(((uint64_t*)pte) + i) |= SetAddr(maxneg);
    }

  }

  //pte = (uint64_t)(((uint64_t*)pte) + GetPTOffset(virt));

  if(((*(uint64_t*)pte) & PE_P) == 0x0ull)
  {
    (*(uint64_t*)pte) |= PE_P;
    (*(uint64_t*)pte) |= PE_RW;
    (*(uint64_t*)pte) |= PE_US;
    (*(uint64_t*)pte) |= PE_PWT;
    (*(uint64_t*)pte) |= PE_PCD;
    //(*(uint64_t*)pte) |= SetAddr(MaskPhyAddr(phy));
    (*(uint64_t*)pte) |= MaskPhyAddr(phy);
  }


  /*{
  Print(L"Done while\n");
  while(1){};
  }*/

  //Print(L"PTE mapping 0x%llx\n", (*(uint64_t*)pte));

  //Print(L"CR3 value: 0x%llX\n", *CR3);
  //Print(L"PML4 value: 0x%llX\n", *(uint64_t*)pml4e);
  //Print(L"PDP value: 0x%llX\n", *(uint64_t*)pdpe);
  //Print(L"PD value: 0x%llX\n", *(uint64_t*)pde);
  //Print(L"PT value: 0x%llX\n", *(uint64_t*)pte);
}

uint8_t checkIdentity(uint64_t phy)
{
  //uint8_t phyvalue = (uint8_t)(*(uint64_t*)phy);
  uint8_t phyvalue = *(uint64_t*)phy;
  uint8_t virtvalue;
  uint64_t pml4e = CR3GetAddr(*CR3);// + GetPML4Offset(phy);
  pml4e = *(((uint64_t*)pml4e) + GetPML4Offset(phy));
  uint64_t pdpe = *(((uint64_t*)GetAddr(pml4e)) + GetPDPOffset(phy));
  uint64_t pde  = *(((uint64_t*)GetAddr(pdpe)) + GetPDOffset(phy));
  uint64_t pte  = *(((uint64_t*)GetAddr(pde)) + GetPTOffset(phy));
  uint64_t pteptr = ((GetAddr(pte)) + GetPhyOffset(phy));
  virtvalue = *(uint64_t*)((GetAddr(pte)) + GetPhyOffset(phy));

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
