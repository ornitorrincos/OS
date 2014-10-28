#ifndef VGA_H
#define VGA_H

#include <efi.h>
#include <efilib.h>

typedef struct _FBinfo
{
  int width;
  int height;

} FBinfo;

void * EFIAPI SetVideoMode(int width, int height, int bitdepth);

#endif // VGA_H
