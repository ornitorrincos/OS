#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "ELF.h"
#include "kernel.h"

extern void BootDisableInterrupts(void);
typedef void (*kfn)(int dummy);

void
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  // Memory Map
    UINTN mapsize = 0;
    UINTN allocsize = 0;
    EFI_MEMORY_DESCRIPTOR * map = NULL;
    UINTN mapkey = 0;
    UINTN descriptorsize = 0;
    UINT32 version = 0;
    
    void * towrite = (void*)0x1000;
  
   InitializeLib(ImageHandle, SystemTable);
   
   BS->SetWatchdogTimer(0, 0, 0, NULL);
   
   uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut); // clear the screen
   Print(L"Firmware Vendor: %s Rev: 0x%08x\n", ST->FirmwareVendor, ST->FirmwareRevision);
   
   ELF * elfheader = (ELF*)kernel_bin;

   if(elfheader->Magic == ELFMAGIC)
      {
        Print(L"Kernel in ELF format found\n");
      }
    if(elfheader->Class == 2)
      {
        Print(L"64 Bits Executable\n");
      }

    if(elfheader->OSABI == 0)
      {
        Print(L"System V ABI\n");
      }

    Print(L"Kernel Entry Point: %x\n", elfheader->EntryPoint);
    
    
    EFI_STATUS memret = EFI_SUCCESS;
    
    
    EFI_STATUS bootstatus = EFI_SUCCESS;
    
    
    EFI_STATUS memst = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapsize, map, &mapkey, &descriptorsize, &version);
    
    if(memst == EFI_BUFFER_TOO_SMALL)
    {
      Print(L"mapSize gotten\n");
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
      Print(L"MemoryMap Gotten\n");
    }
    
    Print(L"Calling ExitBootServices\n");
    
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
	
  BootDisableInterrupts();
  
	*((int*)towrite) = 0xB00B;
	    
	//((EFI_PHYSICAL_ADDRESS)kernel_bin) + elfheader->EntryPoint;
	
	kfn kernel_jump = (void*)((EFI_PHYSICAL_ADDRESS)kernel_bin) + elfheader->EntryPoint;
	
	kernel_jump(0);
	
	//while(1){} // kernel call should go here
	
	// we should never ever reach this point(if kernel exists, it should shutdown the computer)
}
