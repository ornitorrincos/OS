#include "ELF.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>

void PrintELFInfo(ELF * file)
{
  if(file->Magic == ELFMAGIC)
  {
    Print(L"Kernel in ELF format found\n");
  }

  if(file->Class == 2)
  {
    Print(L"64 Bits Executable\n");
  }

  if(file->OSABI == 0)
  {
    Print(L"System V ABI\n");
  }

  Print(L"Kernel Entry Point: %x\n", file->EntryPoint);
}
