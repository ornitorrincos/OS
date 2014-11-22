#ifndef VMMEM_H
#define VMMEM_H

#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI SetVM(IN UINTN size, IN UINTN descriptorsize, IN UINTN descriptorversion, EFI_MEMORY_DESCRIPTOR * map, void * kernel);

#endif // VMMEM_H
