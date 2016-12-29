
#include <stdint.h>

typedef struct _OSDATA
{
  uint32_t Magic; // magic number to check

  uint32_t FBWidth; // with of the framebuffer
  uint32_t FBHeight; // height
  uint32_t PixelSize; // size of each pixel(an rgb pixel might have a bigger size)
  void * FBAddr; // address of the linear framebuffer

  void * MEMMap; // pointer to the system memory map

  void * RAMDisk; // pointer to a ramdisk loaded from the hdd
} OSDATA;

typedef struct _Pixel
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t Z;
} Pixel;

int kmain(void * osdata)
{
  OSDATA* data = (OSDATA*)osdata;

  Pixel* fb = data->FBAddr;

  {
    int i = 0;
    int j = 0;

    for(i = 0; i < 256; ++i)
    {
      for(j = 0; j < 256; ++j)
      {
        Pixel p;
        p.R = 0;
        p.G = 255;
        p.B = 0;
        p.Z = 255;
        fb[j + 800*i] = p; // original: 1024 but 800 seems to work better(maybe we are not setting the display mode correctly
      }
    }
  }

  return 0;
}
