#ifndef MEMORYTYPES_H
#define MEMORYTYPES_H

// memory types to mark the pages as of specific types
// the specification allows custom types that are bigger than 0x80000000

#define MEM_KERNEL  0x80000010
#define MEM_RAMDISK 0x80000020
#define MEM_ARGS    0x80000030
#define MEM_PAGING  0x80000040

#endif // MEMORYTYPES_H
