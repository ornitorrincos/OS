
#include "gdt.h"

#define NULL_DESCRIPTOR 0
#define KERNEL_CODE 1
#define KERNEL_DATA 2
#define USER_CODE 3
#define USER_DATA 4
#define TASK_STATE_SEGMENT_LOW 5
#define TASK_STATE_SEGMENT_HIGH 6

uint64_t gtd[7];

int init_gdt()
{
    create_descriptor(NULL_DESCRIPTOR, 0, 0, 0, 0);
    create_descriptor(KERNEL_CODE, 0, 0xFFFF, 0x9A, 0xA);
    create_descriptor(KERNEL_DATA, 0, 0xFFFF, 0x92, 0xC);
    create_descriptor(USER_CODE, 0, 0xFFFF, 0xFA, 0xA);
    create_descriptor(USER_DATA, 0, 0xFFFF, 0xF2, 0xC);
    create_system_segment_descriptor();

    return 1;
}

void create_descriptor(int type, uint32_t base, uint32_t limit, uint8_t access_byte, uint16_t flag)
{
    // need to redo
    uint64_t descriptor;

    descriptor  =  limit       & 0x000F0000;
    descriptor |= (flag <<  8) & 0x00F0FF00;
    descriptor |= (base >> 16) & 0x000000FF;
    descriptor |=  base        & 0xFF000000;
 
    descriptor <<= 32;
 
    descriptor |= base  << 16;
    descriptor |= limit  & 0x0000FFFF;

    gtd[type] = descriptor;
}

void create_system_segment_descriptor()
{

}
