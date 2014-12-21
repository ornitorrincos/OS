#ifndef PAGING_STRUCT_H
#define PAGING_STRUCT_H

typedef unsigned char uchar; // need to replace this for other more sensible thing or location

// these entries are for 4KB pages

// virtual address pointer structure:

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
  uchar PWT:1;
  uchar PCD:1;
  uchar reserved2:7;
  uchar base_addr:40; // base address to the table of PML4 entries
  uchar reserved3:13;
} CR3;

// PML4E

typedef struct _PML4E
{
  uchar P:1;
  uchar RW:1;
  uchar US:1;
  uchar PWT:1;
  uchar PCD:1;
  uchar A:1;
  uchar ignored:1;
  uchar MBZ:2;
  uchar AVL:3;
  uchar PDPBA:40;
  uchar available:11; // need to look into what values go here
  uchar NX:1;
} PML4E;

// PDPE

typedef struct _PDPE
{
  uchar P:1;
  uchar RW:1;
  uchar US:1;
  uchar PWT:1;
  uchar PCD:1;
  uchar A:1;
  uchar ignored:1;
  uchar zero:1; // zero
  uchar MBZ:1;
  uchar AVL:3;
  uchar PDBA:40;
  uchar available:11;
  uchar NX:1;
} PDPE;

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
} PDE;

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
} PTE;

#endif // PAGING_STRUCT_H

