#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "disk.h"
#include "vga.h"
#include "vmmem.h"
#include "ELF.h"
#include "memorytypes.h"
#include "paging_struct.h"
#include "paging.h"
#include "other.h"

// pixel struct information(hardcoded to what qemu exposes)
typedef struct _Pixel
{
  UINT8 B;
  UINT8 G;
  UINT8 R;
  //UINT8 Z;
} Pixel;

// struct that is going to be passed to the kernel about general system information
typedef struct _OSDATA
{
  UINT32 Magic; // magic number to check

  UINT32 FBWidth; // with of the framebuffer
  UINT32 FBHeight; // height
  UINT32 PixelSize; // size of each pixel(an rgb pixel might have a bigger size)
  void * FBAddr; // address of the linear framebuffer

  void * MEMMap; // pointer to the system memory map

  void * RAMDisk; // pointer to a ramdisk loaded from the hdd
} OSDATA;

extern void BootDisableInterrupts(void); // asm code is not correct(callee doesn't set the stack correctly)
typedef void (*kfn)(OSDATA *);  // typedef to setup the entry point of the kernel and do a "jump" into it



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

  PrintImageAddr(ImageHandle);

  // some uefi implementations time out with their default setting, disable the timer
  BS->SetWatchdogTimer(0, 0, 0, NULL);
   

  //Print(L"Firmware Vendor: %s Rev: 0x%08x\n", ST->FirmwareVendor, ST->FirmwareRevision);

  //while(1){};

  // allocate the datat for the kernel(need to specify memory time not to be a generic loader data type)
  OSDATA * osdata = AllocatePool(sizeof(OSDATA));

  if(osdata == NULL)
  {
    Print(L"Os Data allocation failed\n");
  }

  int32_t kernel_size;

  ELF * kernel = LoadFile(L"kernel.bin", MEM_KERNEL, &kernel_size); // we set the memory type to the one from the kernel

  // print general information about the kers_CR3nel elf header
  //PrintELFInfo(kernel);





  // attempt to allocate the memory map, first try is going to be too small
  // as such the firmware will return the correct size
  EFI_STATUS memret     = EFI_SUCCESS;
  EFI_STATUS bootstatus = EFI_SUCCESS;

  uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version);


  // as the allocation will probably modify the memory map allocate 4kb more(one page)
  // so that the new memory map probably fits
  allocsize = mapsize + 10*4098;

  mapsize = allocsize;

  uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, allocsize, (void**)&map);

  uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version);

  // set virtual addresses in here
  // try some paging
  initCR3();

  //uint64_t address = 0;
  //uint64_t max = 0x400000000ull;//0x20000000;
  EFI_MEMORY_DESCRIPTOR * mapiterator = map;

  uint64_t elements = mapsize/descriptorsize;

  printCR3();

  //Print(L"elements: %d\n", elements);

  for(int entry = 0; entry < elements; ++entry)
  {
    mapiterator = (EFI_MEMORY_DESCRIPTOR*)(((EFI_PHYSICAL_ADDRESS)mapiterator + descriptorsize));
    uint64_t page = mapiterator->PhysicalStart;


    //Print(L"page: 0x%llX\n", page);
    for(int pageentry = 0; pageentry < mapiterator->NumberOfPages; pageentry++)
    {
      //Print(L"SetAddr\n");
      SetVirtualAddress(page, page);
      page += 0x1000;
    }
  }

  // need to map the physical address the kernel is in to -2GB virtual
  //uint64_t startvm = 0xffffffff7fffffffull;
  uint64_t startvm = 0xffffffff80000000ull;
  uint64_t currentvm = 0;

  while(currentvm < kernel_size)
  {
    SetVirtualAddress((uint64_t)kernel + currentvm, startvm + currentvm);

    currentvm += 0x1000;
  }

  //printCR3();

  //Print(L"kernel: 0x%llx\n", kernel);

  PrintELFInfo(kernel);

  // the call to exit boot services tells the firmware we are ready to take control of the system

  // never ever call Print after the next line
  // after the first call boot services can be partially disabled
  // loop until the firmware reports a successful exit, the specification allows for partial shutdowns
  // so more than one call might be necessary(on qemu with OVMF it is)
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

  // as we can't print to the screen the way of showing the return status of this function
  // is to write a red or green square on the top left corner of the screen
  if(SetVM(mapsize, descriptorsize, version, map, kernel) != EFI_SUCCESS)
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

  // we should now set s  Print(L"Calling ExitBootServices\n");oe virtual mapping
  //BootDisableInterrupts();


  //OSDATA * osdata = (OSDATA*)(640*1024);



  osdata->Magic = 0xDDEE;
  osdata->FBWidth = 1024;
  osdata->FBHeight = 768;
  osdata->FBAddr = fb;
  osdata->PixelSize = 24;
  osdata->RAMDisk = NULL;

  // this while only serves not to call the kernel for now
  // after calling exit boot services we can't return to the uefi environment because it's been destroyed
  //

  kernel = (ELF*)0x280000000; // correct virtual pointer to 10GB (Sign: 0  PML4: 0  PDP:10  PD:0  Page:0)

  kfn kernel_jump = NULL;//(void*)((EFI_PHYSICAL_ADDRESS)kernel + kernel->EntryPoint);

  writeCR3();

  // disable interrupts
  __asm__("cli");

  //writeCR3();
  while(1){}

  __asm__("hlt");

  kernel_jump(osdata);

  __asm__("hlt"); // sanity in case the kernel exists, should throw an error somehow
  // not as if the kernel shouldn't have this same code at the end of the main though

  // we should never ever reach this point(if kernel exists, it should shutdown the computer)
}
