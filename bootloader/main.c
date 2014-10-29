#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "disk.h"
#include "vga.h"
#include "vmmem.h"
#include "ELF.h"


extern void BootDisableInterrupts(void);
typedef void (*kfn)();

typedef struct _Pixel
{
  UINT8 R;
  UINT8 G;
  UINT8 B;
  //UINT8 Z;
} Pixel;

typedef struct _OSDATA
{
  UINT32 Magic;

  UINT32 FBWidth;
  UINT32 FBHeight;
  UINT32 PixelSize;
  void * FBAddr;

  void * MEMMap;

  void * RAMDisk;
} OSDATA;

void
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  // Memory Map
  UINTN mapsize               = 0;
  UINTN allocsize             = 0;
  EFI_MEMORY_DESCRIPTOR * map = NULL;
  UINTN mapkey                = 0;
  UINTN descriptorsize        = 0;
  UINT32 version              = 0;

  
  InitializeLib(ImageHandle, SystemTable);
  uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut); // clear the screen
  Pixel * fb = SetVideoMode(1024, 768, 32);



  BS->SetWatchdogTimer(0, 0, 0, NULL);
   

  Print(L"Firmware Vendor: %s Rev: 0x%08x\n", ST->FirmwareVendor, ST->FirmwareRevision);
   
  ELF * kernel = LoadFile(L"kernel.bin");

  PrintELFInfo(kernel);

  EFI_STATUS memret     = EFI_SUCCESS;
  EFI_STATUS bootstatus = EFI_SUCCESS;

  EFI_STATUS memst = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version);



  if(memst == EFI_BUFFER_TOO_SMALL)
  {
    Print(L"MapSize read successful\n");
  }

  allocsize = mapsize + 10*4098;

  mapsize = allocsize;

  EFI_STATUS allocst = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, allocsize, (void**)&map);

  if(allocst == EFI_SUCCESS)
  {
    Print(L"Allocated Space for Memory Map\n");
  }



  memst = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version);

  if(memst == EFI_SUCCESS)
  {
    Print(L"MemoryMap Aquired\n");
  }


  Print(L"Calling ExitBootServices\n");

  // never ever call Print after the next line
  // after the first call boot services are partially disabled
  while((bootstatus = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, mapkey)) != EFI_SUCCESS)
  {

    mapsize = allocsize;

    while((memret = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version)) == EFI_BUFFER_TOO_SMALL)
    {

      allocsize = allocsize + 2*4096; // add 8kb(2 pages)

      uefi_call_wrapper(BS->FreePool, 1, map);

      uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, allocsize, (void**)&map);

      mapsize = allocsize;
    }
  }

  SetCrc(&(SystemTable->Hdr)); // As we exited boot services we need to set the CRC32 again

  if(SetVM(mapsize, descriptorsize, version, map) != EFI_SUCCESS)
  {
    int i = 0;
    int j = 0;

    for(i = 0; i < 256; ++i)
    {
      for(j = 0; j < 256; ++j)
      {
        Pixel p;
        p.R = 255;
        p.G = 0;
        p.B = 0;
        //p.Z = 255;
        fb[j + 1024*i] = p;
      }
    }
  } else
  {
    int i = 0;
    int j = 0;

    for(i = 0; i < 64; ++i)
    {
      for(j = 0; j < 64; ++j)
      {
        Pixel p;
        p.R = 0;
        p.G = 255;
        p.B = 0;
        //p.Z = 255;
        fb[j + 1024*i] = p;
      }
    }
  }

  // we should now set soe virtual mapping
  BootDisableInterrupts();


  OSDATA * osdata = (OSDATA*)(640*1024);

  osdata->Magic = 0xDDEE;
  osdata->FBWidth = 2014;
  osdata->FBHeight = 768;
  osdata->FBAddr = fb;
  osdata->PixelSize = 24;
  osdata->RAMDisk = NULL;

  kfn kernel_jump = (void*)((EFI_PHYSICAL_ADDRESS)kernel + kernel->EntryPoint);

  kernel_jump();

  while(1){} // kernel call should go here

  // we should never ever reach this point(if kernel exists, it should shutdown the computer)
}
