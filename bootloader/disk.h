#ifndef DISK_H
#define DISK_H

#include <efidef.h>

void * EFIAPI LoadFile(CHAR16 * name, UINTN memtype);

#endif // DISK_H
