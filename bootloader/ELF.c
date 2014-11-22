#include "ELF.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>

void EFIAPI PrintELFInfo(ELF * file)
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

  Print(L"LoadAddress: 0x%x\n", file);
  Print(L"Kernel Entry Point: 0x%x\n", file->EntryPoint);

}
