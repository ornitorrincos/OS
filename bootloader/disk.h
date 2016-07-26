#ifndef DISK_H
#define DISK_H

#include <efidef.h>
#include <stdint.h>

void * EFIAPI LoadFile(CHAR16 * name, UINTN memtype, int32_t* filesize);

#endif // DISK_H
