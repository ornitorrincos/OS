#ifndef ELF_H
#define ELF_H

#include <efi.h>

#define ELFMAGIC 0x464C457F

// Elf header struct
typedef struct ELF
{
  UINT32 Magic;
  UINT8  Class;
  UINT8  Data;
  UINT8  Version;
  UINT8  OSABI;
  UINT8  ABIVERSION;

  // Padding 7 bytes
  UINT32 Padding4;
  UINT16 Padding2;
  UINT8  Padding1;
  // 4 + 2 + 1 = 7

  UINT16 Machine;
  UINT32 Version2;
  UINT64 EntryPoint;
  UINT64 Phoff;
  UINT32 Flags;
  UINT16 Ehsize;
  UINT16 Phentsize;
  UINT16 Phnum;
  UINT16 Shentsize;
  UINT16 Shnum;
  UINT16 Shstrndx;
} ELF;

void EFIAPI PrintELFInfo(ELF * file);

#endif
