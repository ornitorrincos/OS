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
    return;
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

  Print(L"Entry point: hx%x\n", header->e_entry);


  char * strtable = elf_str_table(header);
  Print(L"%s\n", strtable);
}

// functions to access section headers
Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr)
{
  return (Elf64_Shdr *)((uintptr_t)hdr + hdr->e_shoff);
}

Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx)
{
  return &elf_sheader(hdr)[idx];
}

char *elf_str_table(Elf64_Ehdr *hdr)
{
  if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
  return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}

