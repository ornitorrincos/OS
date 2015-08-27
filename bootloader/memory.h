#ifndef MEMORY_H
#define MEMORY_H

#include <efi.h>
#include <efilib.h>

#include <stdint.h>

#include "memorytypes.h"

void * EFIAPI AllocatePagesType(IN UINTN size, IN UINTN type);
void bootloader_memset(void *, uint64_t size);

#endif
