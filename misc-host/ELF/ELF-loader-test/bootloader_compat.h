#ifndef BOOTLOADER_COMPAT_H
#define BOOTLOADER_COMPAT_H

// partial API of UEFI/bootloader

#include <wchar.h>
#include <stdint.h>

typedef int (*UEFIPrint)(const wchar_t *, ...);

extern UEFIPrint Print;

// basic UEFI datatypes
typedef uint8_t BOOLEAN;
typedef int8_t INT8;
typedef uint8_t UINT8;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UINT32;
typedef int64_t INT64;
typedef uint64_t UINT64;
typedef char CHAR8;
typedef uint16_t CHAR16;

#endif // BOOTLOADER_COMPAT_H

