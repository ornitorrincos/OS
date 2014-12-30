#ifndef MEMORY_H
#define MEMORY_H

#include <efi.h>
#include <efilib.h>

#include "memorytypes.h"

void * EFIAPI AllocatePagesType(IN UINTN size, IN UINTN type);

#endif
