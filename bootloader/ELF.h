#ifndef ELF_H
#define ELF_H

#include <efi.h>

#define ELFMAGIC 0x464C457F

// Elf header struct
typedef struct ELF
{
  UINT8 e_ident[16];
  UINT16 e_type;
  UINT16 e_machine;
  UINT32 e_version;
  UINT64 e_entry;
  UINT64 e_phoff;
  UINT64 e_shoff;
  UINT32 e_flags;
  UINT16 e_ehsize;
  UINT16 e_phentsize;
  UINT16 e_phnum;
  UINT16 e_shentsize;
  UINT16 e_shnum;
  UINT16 e_shstrndx;
} ELF;

typedef struct PH
{
  UINT32 p_type;
  UINT32 p_flags;
  UINT64 p_offset;
  UINT64 p_vaddr;
  UINT64 p_paddr;
  UINT64 p_filesz;
  UINT64 p_memsz;
  UINT64 p_align;
} PH;

void EFIAPI PrintELFInfo(ELF * file);

#endif
