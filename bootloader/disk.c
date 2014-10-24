#include <efi.h>
#include <efilib.h>

void * LoadFile(CHAR16 * name)
{
  // Get Protocol handle
  EFI_GUID LoadFileProtocolGuid = LOAD_FILE_PROTOCOL;

  UINTN BufferSize = 4096;
  EFI_HANDLE * HandleBuffer = AllocatePool(BufferSize);
  UINTN DummySearchKey;

  EFI_STATUS ret = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &LoadFileProtocolGuid, &DummySearchKey, &BufferSize, HandleBuffer);

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

  ret = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &LoadFileProtocolGuid, &DummySearchKey, &BufferSize, HandleBuffer);

  if(ret == EFI_SUCCESS)
  {
    Print(L"And Loaded every handle to memory\n");
    UINTN HandleNum = BufferSize/sizeof(EFI_HANDLE);
    Print(L"Number of handles: %d\n", HandleNum);
  }

  // From the handle get the protocol

  // read the file


  // return a pointer to the file

  // if anything fails return NULL
  return NULL;
}

