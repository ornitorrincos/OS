#ifndef PAGING_STRUCT_H
#define PAGING_STRUCT_H

#include "bootloader_compat.h"
#include <stdint.h>

typedef UINT64 uchar; // need to replace this for other more sensible thing or location

UINT64 GetVMCPUID(); // implemented in misc.s

extern uint64_t maxneg;

typedef struct _CPUIDsizes
{
  UINT64 PhysicalAddress:8;
  UINT64 VirtualAddress:8;
  UINT64 Padding:48;
} CPUIDsizes;

// these entries are for 4KB pages


//                                Virtual pointer bits breakdown (4KB pages)
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |     63 - 48   |   47 - 39   |   38 - 30  |  29 - 21  |  20 - 12  |       11 - 0          |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |   sign extend | PML4 offset | PDP offset | PD offset | PT offset | physical page offset  |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+

//                                Virtual pointer bits breakdown (2MB pages)
//    +---------------+-------------+------------+-----------+-----------------------------------+
//    |     63 - 48   |   47 - 39   |   38 - 30  |  29 - 21  |              20 - 0               |
//    +---------------+-------------+------------+-----------+-----------------------------------+
//    |   sign extend | PML4 offset | PDP offset | PD offset |        physical page offset       |
//    +---------------+-------------+------------+-----------+-----------------------------------+


// paging structure:

// CR3
// the contents of this struct go into CR3
typedef struct __attribute__((packed))
{
  uint64_t reserved1:3;
  uint64_t PWT:1;        // page level writethough
  uint64_t PCD:1;        // page level cache disable
  uint64_t reserved2:7;
  uint64_t base_addr:40; // base address to the table of PML4 entries
  uint64_t reserved3:13;
} s_CR3;

// Virtual Pointer
#define VP_OFF_PHY   0ull
#define VP_OFF_PT   12ull
#define VP_OFF_PD   21ull
#define VP_OFF_PDP  30ull
#define VP_OFF_PML4 39ull
#define VP_OFF_SIGN 48ull

#define GetPhyOffset(x)  (x & 0xFFFull)
#define GetPTOffset(x)   ((x >> VP_OFF_PT) & 0x1FFull)
#define GetPDOffset(x)   ((x >> VP_OFF_PD) & 0x1FFull)
#define GetPDPOffset(x)  ((x >> VP_OFF_PDP) & 0x1FFull)
#define GetPML4Offset(x) ((x >> VP_OFF_PML4) & 0x1FFull)
#define GetSignOffset(x) ((x >> VP_OFF_SIGN) & 0xFFFFull)


// CR3
#define CR3_PWT (1ull << 3ull)
#define CR3_PCD (1ull << 4ull)
#define CR3_ADDR_SHIFT 12ull
#define CR3_ERASE (~(maxneg << CR3_ADDR_SHIFT))

#define CR3GetAddr(x) ((x >> CR3_ADDR_SHIFT) & maxneg)
#define CR3SetAddr(x) ((x & maxneg) << CR3_ADDR_SHIFT)

// Page Entry
#define PE_P   (1ull <<  0ull)
#define PE_RW  (1ull <<  1ull)
#define PE_US  (1ull <<  2ull)
#define PE_PWT (1ull <<  3ull)
#define PE_PCD (1ull <<  4ull)
#define PE_A   (1ull <<  5ull)
#define PE_D   (1ull <<  6ull)
#define PE_PS  (1ull <<  7ull)
#define PE_G   (1ull <<  8ull)
#define PE_BA  (1ull << 12ull)
#define PE_NX  (1ull << 63ull)
#define PE_ADDR_SHIFT 12ull
#define PE_ERASE      (~(maxneg << PE_ADDR_SHIFT))

#define MaskTable(x)   (x & maxneg)
#define MaskPhyAddr(x) (x & 0xFFFFFFF000ull)
#define SetAddr(x)  (x << PE_ADDR_SHIFT)
#define GetAddr(x)  (x >> PE_ADDR_SHIFT)
//#define GetAddr(x)  (x & ~0xFFFull)



// PML4E

typedef struct __attribute__((packed))
{
  uint64_t P:1; // present bit
  uint64_t RW:1; // read write (if 0 write protected)
  uint64_t US:1; // user/supervisor if 0 no user mode access
  uint64_t PWT:1; // page level write though
  uint64_t PCD:1; // page level cache disable
  uint64_t A:1; // accesed
  uint64_t PS:1; // value must be 1 for 1GB pages
  uint64_t MBZ:2;
  uint64_t AVL:3;
  uint64_t PDPBA:40; // physical address of the table pointed by this entry
  uint64_t available:11; // need to look into what values go here
  uint64_t NX:1; // execute disable
} s_PML4E;



// PDPE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t PS:1; // value must be 1 for 2MB pages
  uint64_t zero:1; // zero
  uint64_t MBZ:1;
  uint64_t AVL:3;
  uint64_t PDBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PDPE;

// PDE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t ignored1:1;
  uint64_t zero:1;
  uint64_t ignored2:1;
  uint64_t AVL:3;
  uint64_t PTBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PDE;

// PTE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t D:1;
  uint64_t PAT:1;
  uint64_t G:1;
  uint64_t AVL:3;
  uint64_t PPBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PTE;

typedef struct __attribute__((packed))
{
  uint64_t offset:12;
  uint64_t PT:9;
  uint64_t PD:9;
  uint64_t PDP:9;
  uint64_t PML4:9;
  uint64_t sign:16;
} s_VPTR;

#endif // PAGING_STRUCT_H

