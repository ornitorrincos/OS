#ifndef MEMORYTYPES_H
#define MEMORYTYPES_H

#include <efi.h>
#include <efilib.h>

// memory types to mark the pages as of specific types
// the specification allows custom types that are bigger than 0x80000000

UINTN MEM_KERNEL  = 0x80000010;
UINTN MEM_RAMDISK = 0x80000020;
UINTN MEM_ARGS    = 0x80000030;

#endif // MEMORYTYPES_H
