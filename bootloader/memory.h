#ifndef MEMORY_H
#define MEMORY_H

#include <efi.h>
#include <efilib.h>

void * EFIAPI AllocatePages2(IN UINTN size);

#endif
