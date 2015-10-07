#ifndef BOOTLOADER_COMPAT_H
#define BOOTLOADER_COMPAT_H

// partial API of UEFI/bootloader

#include <wchar.h>
#include <stdint.h>

#define EFIAPI
#define IN
#define OUT

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
typedef wchar_t CHAR16;
typedef uint64_t UINTN;

typedef uint64_t* EFI_PHYSICAL_ADDRESS;
typedef uint32_t EFI_STATUS;

#define EFI_SUCCESS 0

// UEFI native functions
typedef int (*UEFIPrint)(const wchar_t *, ...);
extern UEFIPrint Print;

// Bootloader defined
void * EFIAPI LoadFile(char * name, UINTN memtype);

#endif // BOOTLOADER_COMPAT_H

