#include "elfinfoprint.h"

#include "bootloader_compat.h"


bool IsElf(Elf64_Ehdr * header)
{
  if(!header)
  {
    return false;
  }

  if(header->e_ident[EI_MAG0] != ELFMAG0) {
      return false;
    }
    if(header->e_ident[EI_MAG1] != ELFMAG1) {
      return false;
    }
    if(header->e_ident[EI_MAG2] != ELFMAG2) {
      return false;
    }
    if(header->e_ident[EI_MAG3] != ELFMAG3) {
      return false;
    }

    return true;
}

void ElfInfoPrint(Elf64_Ehdr * header)
{
  // check if actual elf file
  if(!IsElf(header))
  {
    Print(L"Not an ELF File\n");
  }

  if(header->e_type == ET_EXEC)
  {
    Print(L"Executable file\n");
  }

  if(header->e_machine == EM_X86_64)
  {
    Print(L"AMD64 Machine\n");
  } else
  {
    Print(L"Unrecorginzed Machine\n");
  }

}
