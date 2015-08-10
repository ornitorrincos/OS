#ifndef PAGING_STRUCT_H
#define PAGING_STRUCT_H

#include <efi.h>
#include <efilib.h>

typedef UINT64 uchar; // need to replace this for other more sensible thing or location

UINT64 GetVMCPUID(); // implemented in misc.s

typedef struct _CPUIDsizes
{
  UINT64 PhysicalAddress:8;
  UINT64 VirtualAddress:8;
  UINT64 Padding:48;
} CPUIDsizes;

// these entries are for 4KB pages


//                                Virtual pointer bits breakdown
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |     63 -48    |   47 - 39   |   38 - 30  |  29 - 21  |  20 - 12  |       11 - 0          |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |   sign extend | PML4 offset | PDP offset | PD offset | PT offset | physical page offset  |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+


// paging structure:

// CR3
// the contents of this struct go into CR3
typedef struct _CR3
{
  uchar reserved1:3;
  uchar PWT:1;        // page level writethough
  uchar PCD:1;        // page level cache disable
  uchar reserved2:7;
  uchar base_addr:40; // base address to the table of PML4 entries
  uchar reserved3:13;
} s_CR3;

// PML4E

typedef struct _PML4E
{
  uchar P:1; // present bit
  uchar RW:1; // read write (if 0 write protected)
  uchar US:1; // user/supervisor if 0 no user mode access
  uchar PWT:1; // page level write though
  uchar PCD:1; // page level cache disable
  uchar A:1; // accesed
  uchar PS:1; // value must be 1 for 1GB pages
  uchar MBZ:2;
  uchar AVL:3;
  uchar PDPBA:40; // physical address of the table pointed by this entry
  uchar available:11; // need to look into what values go here
  uchar NX:1; // execute disable
} s_PML4E;

// PDPE

typedef struct _PDPE
{
  uchar P:1;
  uchar RW:1;
  uchar US:1;
  uchar PWT:1;
  uchar PCD:1;
  uchar A:1;
  uchar PS:1; // value must be 1 for 2MB pages
  uchar zero:1; // zero
  uchar MBZ:1;
  uchar AVL:3;
  uchar PDBA:40;
  uchar available:11;
  uchar NX:1;
} s_PDPE;

// PDE

typedef struct _PDE
{
  uchar P:1;
  uchar RW:1;
  uchar US:1;
  uchar PWT:1;
  uchar PCD:1;
  uchar A:1;
  uchar ignored1:1;
  uchar zero:1;
  uchar ignored2:1;
  uchar AVL:3;
  uchar PTBA:40;
  uchar available:11;
  uchar NX:1;
} s_PDE;

// PTE

typedef struct _PTE
{
  uchar P:1;
  uchar RW:1;
  uchar US:1;
  uchar PWT:1;
  uchar PCD:1;
  uchar A:1;
  uchar D:1;
  uchar PAT:1;
  uchar G:1;
  uchar AVL:3;
  uchar PPBA:40;
  uchar available:11;
  uchar NX:1;
} s_PTE;

// unions

union CR3
{
  s_CR3 bits;
  long value;
};

union PML4E
{
  s_PML4E bits;
  long value;
};

union PDPE
{
  s_PDPE bits;
  long value;
};

union PDE
{
  s_PDE bits;
  long value;
};

union PTE
{
  s_PTE bits;
  long value;
};

#endif // PAGING_STRUCT_H

