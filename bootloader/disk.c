#include <efi.h>
#include <efilib.h>

#include "disk.h"
#include "memory.h"

UINTN EFIAPI ceil2(float x)
{
  UINTN ret = x;
  if (x - (int)x > 0) {
      x += 1;
      ret = (int)x;
  }

  return ret;
}

void * EFIAPI LoadFile(CHAR16 * name, UINTN memtype)
{
  EFI_GUID EfiSimpleFileSystemGuid = SIMPLE_FILE_SYSTEM_PROTOCOL;
  EFI_FILE_IO_INTERFACE * FSInterface = NULL;
  struct _EFI_FILE_HANDLE * fsroot = NULL;

  EFI_STATUS localteprotocolstat = uefi_call_wrapper(BS->LocateProtocol, 3, &EfiSimpleFileSystemGuid, NULL, &FSInterface);

  if(localteprotocolstat != EFI_SUCCESS)
  {
    Print(L"Failed to locate protocol\n");
  }

  EFI_STATUS openvolumestat = uefi_call_wrapper(FSInterface->OpenVolume, 2, FSInterface, &fsroot);

  if(openvolumestat != EFI_SUCCESS)
  {
    Print(L"FAiled to open the volume\n");
  }

  struct _EFI_FILE_HANDLE * fp = NULL;

  EFI_STATUS openstat = uefi_call_wrapper(fsroot->Open, 5, fsroot, &fp, name, (UINT64)1, (UINT64)1);



  if(openstat != EFI_SUCCESS)
  {
    Print(L"Failed opening the file\n");
  }


  UINTN infosize = 1024;
  EFI_FILE_INFO * info = AllocatePool(infosize);
  EFI_GUID infoguid = EFI_FILE_INFO_ID;

  EFI_STATUS infostatus = uefi_call_wrapper(fp->GetInfo, 4, fp, &infoguid, &infosize, info);

  if(infostatus != EFI_SUCCESS)
  {
    Print(L"Failed to get FileInfo\n");
  } else
  {
    Print(L"FileSize: %d\n", info->FileSize);
  }

  UINTN size = info->FileSize;
  void * data = NULL;

  //data = AllocatePool(size);
  //EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size, (void**)&data);
  float pages = (size/1024)/4;

  UINTN ipages = ceil2(pages);
  EFI_PHYSICAL_ADDRESS tmp;

  EFI_STATUS allocstatus = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, memtype, ipages, &tmp);

  data = (void*)tmp;

  //data = AllocatePages2(size);

  //if(data == NULL)
  if(allocstatus != EFI_SUCCESS)
  {
    Print(L"Failed to get Kernel memory\n");
  }

  // phoenix lies, must specify the size myself
  uefi_call_wrapper(fp->Read, 3, fp, &size, data);

  Print(L"Read Size: %d\n", size);

  uefi_call_wrapper(fp->Close, 1, fp);

  return data;
}

