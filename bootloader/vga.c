#include <efi.h>
#include <efilib.h>

#include "vga.h"

void * EFIAPI SetVideoMode(int width, int height, int bitdepth)
{
  // find the location of the Graphics Output protocol to aquire a linear framebuffer
  EFI_GUID EfiGOPGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  struct _EFI_GRAPHICS_OUTPUT_PROTOCOL * Graphics = NULL;

  Print(L"Searching for GOP\n");

  EFI_STATUS localteprotocolstat = uefi_call_wrapper(BS->LocateProtocol, 3, &EfiGOPGuid, NULL, &Graphics);

  if(localteprotocolstat != EFI_SUCCESS)
  {
    Print(L"Failed to locate GOP\n");
  }

  // we now search for a 1024x768 mode with a bit depth of 32bits
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE * GOPMode;

  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * GOPInfo = NULL;

  GOPMode = Graphics->Mode;

  UINT32 MaxModes = GOPMode->MaxMode;

  UINTN infosize = 0;

  uefi_call_wrapper(Graphics->QueryMode, 4, Graphics, 0, &infosize, &GOPMode);

  Print(L"Size of GOP Info: %d\n", infosize);

  // print information about the available modes
  UINT32 i;
  for(i = 0; i < MaxModes; ++i)
  {
    uefi_call_wrapper(Graphics->QueryMode, 4, Graphics, i, &infosize, &GOPInfo);

    switch(GOPInfo->PixelFormat)
    {
      case PixelRedGreenBlueReserved8BitPerColor:
        Print(L"RGBZ\n");
        break;
      case PixelBlueGreenRedReserved8BitPerColor:
        Print(L"BGRZ\n");
        break;
      case PixelBitMask:
        Print(L"By Mask\n");
        break;
      case PixelBltOnly:
        Print(L"Only Blt\n"); // this mode doesn't expose a linear framebuffer
      default:
        Print(L"Unknown\n");
        break;
    }
    Print(L"Mode %d:   Width: %d Height: %d\n", i, GOPInfo->HorizontalResolution, GOPInfo->VerticalResolution);

  }

  uefi_call_wrapper(Graphics->SetMode, 2, Graphics, 2); // hardcoded to mode 2(1024x768), this is only tue for the cirrus vga that qemu exposes
  uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut); // clear the screen
  //uefi_call_wrapper(Graphics->QueryMode, 4, Graphics, i, &infosize, &GOPInfo);
  Print(L"Width: %d Height: %d\n", Graphics->Mode->Info->HorizontalResolution, Graphics->Mode->Info->VerticalResolution);
  switch(Graphics->Mode->Info->PixelFormat)
  {
    case PixelRedGreenBlueReserved8BitPerColor:
      Print(L"RGBZ\n");
      break;
    case PixelBlueGreenRedReserved8BitPerColor:
      Print(L"BGRZ\n");
      break;
    case PixelBitMask:
      Print(L"By Mask\n");
      break;
    case PixelBltOnly:
      Print(L"Only Blt\n");
    default:
      Print(L"Unknown\n");
      break;
  }

  Print(L"RedBitmask: %d\n", Graphics->Mode->Info->PixelInformation.RedMask);
  Print(L"GreenBitmask: %d\n", Graphics->Mode->Info->PixelInformation.GreenMask);
  Print(L"BlueBitmask: %d\n", Graphics->Mode->Info->PixelInformation.BlueMask);
  Print(L"ReservedBitmask: %d\n", Graphics->Mode->Info->PixelInformation.ReservedMask);
  Print(L"Pixels Per ScanLine: %d\n", Graphics->Mode->Info->PixelsPerScanLine);

  // pixels seem to be 4 bytes wide(32bits)
  // format seems to be RGB Reserved


  Print(L"FrameBuffer Size: %d\n", Graphics->Mode->FrameBufferSize);
  Print(L"FrameBuffer Addr: %d\n", Graphics->Mode->FrameBufferBase);

  return (void*)Graphics->Mode->FrameBufferBase;
}

