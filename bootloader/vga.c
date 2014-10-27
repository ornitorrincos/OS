#include <efi.h>
#include <efilib.h>

#include "vga.h"

void * EFIAPI SetVideoMode(int width, int height, int bitdepth)
{
  EFI_GUID EfiGOPGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  struct _EFI_GRAPHICS_OUTPUT_PROTOCOL * Graphics = NULL;

  Print(L"Searching for GOP\n");

  EFI_STATUS localteprotocolstat = uefi_call_wrapper(BS->LocateProtocol, 3, &EfiGOPGuid, NULL, &Graphics);

  if(localteprotocolstat != EFI_SUCCESS)
  {
    Print(L"Failed to locate GOP\n");
  }

  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE * GOPMode;

  GOPMode = Graphics->Mode;

  if(GOPMode != NULL)
  {
    Print(L"Total Modes: %d\n", GOPMode->MaxMode);
    Print(L"Width: %d Height: %d\n", GOPMode->Info->HorizontalResolution, GOPMode->Info->VerticalResolution);
  }

  return NULL;
}

