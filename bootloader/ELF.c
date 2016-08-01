#include "ELF.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>

void EFIAPI PrintELFInfo(ELF * file)
{
  //if(file->Magic == ELFMAGIC)
  //{
  //  Print(L"Kernel in ELF format found\n");
  //}

  //if(file->Class == 2)
  //{
  //  Print(L"64 Bits Executable\n");
  //}

  //if(file->OSABI == 0)
  //{
  //  Print(L"System V ABI\n");
  //}

  Print(L"LoadAddress: 0x%x\n", file);
  //Print(L"Kernel Entry Point: 0x%x\n", file->EntryPoint);

  //Print(L"program Header offset: %d\n", file->Phoff);
  //Print(L"Program Heder Size: %d\n", file->Phentsize);
  //Print(L"Program Header number: %d\n", file->Phnum);

  Print(L"Kernel Entry Point: 0x%llx\n", file->e_entry);

  Print(L"program Header offset: %d\n", file->e_phoff);
  Print(L"Program Heder Size: %d\n", file->e_phentsize);
  Print(L"Program Header number: %d\n", file->e_phnum);

  PH* ph = (PH*)(((uint64_t)file) + file->e_phoff);

  Print(L"PHAddress: 0x%x\n", ph);
  Print(L"LOAD offset: 0x%llx\n", ph->p_offset);
  Print(L"LOAD vaddr: 0x%llx\n", ph->p_vaddr);
  Print(L"LOAD size in file: 0x%llx\n", ph->p_filesz);
  Print(L"LOAD size in memory: 0x%llx\n", ph->p_memsz);
}
