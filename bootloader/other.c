#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "other.h"

void PrintImageAddr(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;
  EFI_STATUS status;

  status = uefi_call_wrapper(BS->HandleProtocol,
                             3,
                             image,
                             &LoadedImageProtocol,
                             (void **)&loaded_image);

  if (EFI_ERROR(status))
  {
    Print(L"handleprotocol: %r\n", status);
  }

  Print(L"Image base: 0x%lx\n", loaded_image->ImageBase);
}

