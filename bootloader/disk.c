#include <efi.h>
#include <efilib.h>

void * LoadFile(CHAR16 * name)
{
  // Get Protocol handle
  EFI_GUID SimpleFileSystemProtocolGuid = SIMPLE_FILE_SYSTEM_PROTOCOL;

  UINTN BufferSize = 4096;
  EFI_HANDLE * HandleBuffer = AllocatePool(BufferSize);
  UINTN DummySearchKey;

  //I_GUID = SIMPLE_FILE_SYSTEM_PROTOCOL;
  //SIMPLE_FILE_SYSTEM_PROTOCOL FileProt;

  EFI_STATUS ret = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &SimpleFileSystemProtocolGuid, &DummySearchKey, &BufferSize, HandleBuffer);

  if(ret == EFI_BUFFER_TOO_SMALL)
  {
    Print(L"We found the FileSystem Protocol handle\n");
    Print(L"But the Buffer is too small\n");
    FreePool(HandleBuffer);
    HandleBuffer = AllocatePool(BufferSize);
  } else
  {
    switch(ret)
    {
      case EFI_NOT_FOUND:
        Print(L"Protocol Not found\n");
        break;
      case EFI_INVALID_PARAMETER:
        Print(L"Invalid parameter\n");
        break;
    }
  }



  ret = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &SimpleFileSystemProtocolGuid, &DummySearchKey, &BufferSize, HandleBuffer);

  if(ret == EFI_SUCCESS)
  {
    Print(L"And Loaded every handle to memory\n");
    UINTN HandleNum = BufferSize/sizeof(EFI_HANDLE);
    Print(L"Number of handles: %d\n", HandleNum);
  }

  // From the handle get the protocol

  EFI_FILE_IO_INTERFACE * fileioprot;

  EFI_FILE * fileprot;

  //uefi_call_wrapper(BS->OpenProtocol, 6, *HandleBuffer, );
  EFI_STATUS fileprotstat= uefi_call_wrapper(BS->HandleProtocol, 3, *HandleBuffer, &SimpleFileSystemProtocolGuid, &fileioprot);

  if(fileprotstat != EFI_SUCCESS)
  {
    Print(L"Protocol FAILED Load\n");
  } else
  {
    Print(L"Protocol Loaded\n");
  }

  uefi_call_wrapper(fileioprot->OpenVolume, 2, fileioprot, &fileprot);

  EFI_FILE * file;
  // open the file
  EFI_STATUS filefound = uefi_call_wrapper(fileprot->Open, 5, fileprot, &file, name, EFI_FILE_MODE_READ, EFI_FILE_VALID_ATTR);

  if(filefound == EFI_SUCCESS)
  {
    Print(L"File Opened\n");
  }

  // read the file

  UINTN filesize = 0;
  void * filedata = NULL;

  EFI_STATUS fileloadstatus;

  if((fileloadstatus = uefi_call_wrapper(file->Read, 3, file, filesize, &filedata)) == EFI_BUFFER_TOO_SMALL)
  {
    Print(L"Buffer for file too small\n");
  } else
  {
    switch(fileloadstatus)
    {
      case EFI_NO_MEDIA:
        Print(L"No Media\n");
        break;
      case EFI_DEVICE_ERROR:
        Print(L"Device Error\n");
        break;
      case EFI_VOLUME_CORRUPTED:
        Print(L"Volume Corrupted\n");
        break;
      case EFI_SUCCESS:
        Print(L"Loaded correctly ????\n");
        Print(L"FileSize: %d\n", filesize);
        break;
    }
  }

  filedata = AllocatePool(filesize);

  if(uefi_call_wrapper(file->Read, 3, file, filesize, &filedata) == EFI_SUCCESS)
  {
    return filedata;
  }

  // return a pointer to the file

  // if anything fails return NULL
  return NULL;
}

